#include "lexer.h"
#include "arena_allocator.h"
#include "instruction_format_table.h"
#include "logger.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARENA_SIZE (1 * 1024 * 1024) // 1 MB
#define INITIAL_TOKEN_CAPACITY 256

// ------------------------
//  OPCODE LIST
// ------------------------

const char* const OPCODES[] = {"PRINT_CHR", "PRINT_STR", "MOV",  "LOAD_ADDR", "ADD", "SUB", "MUL",
                               "DIV",       "MOD",       "AND",  "OR",        "NOT", "CMP", "JZ",
                               "JNZ",       "JEQ",       "JGT",  "JGE",       "JLT", "JLE", "JMP",
                               "CALL",      "RET",       "PUSH", "POP",       "HALT"};

const int NUM_OPCODES = sizeof(OPCODES) / sizeof(OPCODES[0]);

const char* const REGISTERS[] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "SP", "BP", "HP"};

const int NUM_REGISTERS = sizeof(REGISTERS) / sizeof(REGISTERS[0]);

const char* const DIRECTIVES[] = {".START", ".DATA", ".RODATA", ".GLOBAL"};

const int NUM_DIRECTIVES = sizeof(DIRECTIVES) / sizeof(DIRECTIVES[0]);

// ------------------------
// HELPER FUNCTIONS
// ------------------------
bool isRegister(const char* s)
{
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        if (strcmp(s, REGISTERS[i]) == 0)
        {
            return true;
        }
    }
    return false;
}
bool isOpcode(const char* s)
{
    for (int i = 0; i < NUM_OPCODES; i++)
    {
        if (strcmp(s, OPCODES[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

Register getRegisterType(const char* s)
{
    for (int i = 0; i < NUM_REGISTERS; i++)
    {
        if (strcmp(s, REGISTERS[i]) == 0)
        {
            return (Register) i;
        }
    }
    return REG_UNKNOWN;
}

Directive getDirectiveType(const char* s)
{
    for (int i = 0; i < NUM_DIRECTIVES; i++)
    {
        if (strcmp(s, DIRECTIVES[i]) == 0)
        {
            return (Directive) i;
        }
    }
    return DIRECT_UNKNOWN;
}

// ------------------------
// LEXER
// ------------------------

char** getLexemesInLine(MemoryArena* arena, char* line, int* count)
{
    // Allocate space for up to 100 char* pointers on the arena
    char** lexemes = (char**) arena_alloc(arena, 100 * sizeof(char*));
    *count         = 0;
    char* current  = line;

    while (*current)
    {
        // 1. Skip leading whitespace
        while (*current != '\n' && isspace((unsigned char) *current))
        {
            current++;
        }

        if (*current == '\0' || *current == '\r')
        {
            break; // End of line/string
        }

        // 2. Handle Comments (Assuming ';' or '#' starts a comment)
        if (*current == ';' || *current == '#')
        {
            break;
        }

        // 3. Handle Single-Character Separators (Comma and Newline)
        if (*current == ',' || *current == '\n')
        {
            if (*count <= 10)
            {
                // Allocate 2 bytes (1 for char, 1 for null terminator)
                lexemes[*count]    = (char*) arena_alloc(arena, 2);
                lexemes[*count][0] = *current;
                lexemes[*count][1] = '\0';
                (*count)++;
            }
            else
            {
                LOG_ERROR("Exceeded maximum number of 10 lexemes per line.");
                break;
            }
            current++;
            continue;
        }

        // 4. Handle Main Lexemes (Identifiers, Numbers, Opcodes, Registers, Literals)
        char* start = current;

        // Find the end of the lexeme: stop at whitespace, comma, or newline/carriage return
        while (*current && !isspace((unsigned char) *current) && *current != ',' &&
               *current != '\n' && *current != '\r')
        {
            current++;
        }

        size_t len = current - start;
        if (len > 0)
        {
            if (*count < 100)
            {
                // Allocate on the arena and copy the lexeme
                lexemes[*count] = (char*) arena_alloc(arena, len + 1);
                strncpy(lexemes[*count], start, len);
                lexemes[*count][len] = '\0';
                (*count)++;
            }
            else
            {
                LOG_ERROR("Exceeded maximum number of 100 lexemes per line.");
                break;
            }
        }
    }
    return lexemes;
}

bool is_ident_start(char c) { return isalpha((unsigned char) c) || c == '_'; }
bool is_ident_char(char c) { return isalnum((unsigned char) c) || c == '_'; }

Token* lexer(MemoryArena* arena, char** line, int count)
{
    // Use calloc to initialize Token array to zero (including TokenValue union)
    Token* tokens = (Token*) arena_calloc(arena, count, sizeof(Token));
    if (tokens == NULL)
    {
        LOG_ERROR("Could not allocate memory for tokens.\n");
        return NULL;
    }

    for (int i = 0; i < count; i++)
    {
        Token token  = {0}; // Initialize token to zero to avoid garbage in padding/union
        char* lexeme = line[i];

        if (*lexeme)
        {

            if (is_ident_start(*lexeme) && *lexeme != 'R' && !isdigit(*(lexeme + 1)) &&
                *lexeme != '.')
            {
                char* start = lexeme;
                lexeme++;
                while (is_ident_char(*lexeme))
                {
                    lexeme++;
                }
                token.kind             = TOK_IDENTIFIER;
                token.lexeme           = start;
                token.value.identifier = start;
            }
            else if (isRegister(lexeme))
            {
                token.kind      = TOK_REGISTER;
                token.value.reg = getRegisterType(lexeme);
                token.lexeme    = lexeme;
            }
            else
            {
                // --- LITERAL / SEPARATOR / SYMBOL / LABEL HANDLING ---
                // Number Literal (Integer)
                // char* hex_literal_part = arena_alloc(arena, sizeof(lexeme + 2));
                // if (hex_literal_part == NULL) {
                //     LOG_ERROR("Failed to allocate space for hex_literal_part variable\n");
                //     return NULL;
                // }
                // strncpy(hex_literal_part, lexeme + 2, strlen(lexeme + 2));
                bool is_negetive_int_literal =
                    (lexeme[0] == '-' && isdigit((unsigned char) lexeme[1]));
                bool is_hex_literal =
                    (lexeme[0] == '0' && lexeme[1] == 'x' && isdigit((unsigned char) lexeme[2]));
                if (isdigit((unsigned char) lexeme[0]) || is_negetive_int_literal || is_hex_literal)
                {

                    token.kind = TOK_LITERAL;
                    // strtoll correctly handles signed long long conversion
                    if (is_hex_literal)
                    {
                        long value = strtoll((const char*) lexeme, NULL, 16);
                        if (value > UINT32_MAX)
                        {
                            LOG_ERROR("Literal longer than 32 bits is not supported.");
                            value = UINT32_MAX;
                        }
                        token.value.literal.value.longValue = value;
                    }
                    else
                    {
                        long long value = strtoll((const char*) lexeme, NULL, 16);
                        if (value > UINT32_MAX)
                        {
                            LOG_ERROR("Literal longer than 32 bits is not supported.");
                            value = UINT32_MAX;
                        }
                        token.value.literal.value.longValue = value;
                    }
                    token.lexeme = lexeme;
                }
                // Character Literal (e.g., 'a')
                else if (lexeme[0] == '\'' && lexeme[2] == '\'' && strlen(lexeme) == 3)
                {
                    token.kind                          = TOK_LITERAL;
                    token.value.literal.value.charValue = lexeme[1];
                    token.lexeme                        = lexeme;
                }
                // String Literal (e.g., "hello")
                else if (lexeme[0] == '\"' && lexeme[strlen(lexeme) - 1] == '\"' &&
                         strlen(lexeme) >= 2)
                {
                    token.kind     = TOK_LITERAL;
                    size_t str_len = strlen(lexeme) - 2;

                    // Allocate space for the string contents (excluding quotes) in the Arena
                    token.value.literal.value.stringValue = (char*) arena_alloc(arena, str_len + 1);
                    // Copy content from lexeme[1] for str_len characters
                    strncpy(token.value.literal.value.stringValue, lexeme + 1, str_len);
                    token.value.literal.value.stringValue[str_len] = '\0';
                    token.lexeme                                   = lexeme;
                }
                // Separator (Comma)
                else if (strcmp(lexeme, ",") == 0)
                {
                    token.kind      = TOK_SEPARATOR;
                    token.value.sep = SEP_COMMA;
                    token.lexeme    = lexeme;
                }
                // Separator (End of Line)
                else if (strcmp(lexeme, "\n") == 0)
                {
                    token.kind      = TOK_SEPARATOR;
                    token.value.sep = SEP_EOL;
                    token.lexeme    = lexeme;
                }
                // Directives (starts with '.')
                else if (lexeme[0] == '.' && lexeme[1] != '\0' && strlen(lexeme) > 3)
                {
                    token.kind            = TOK_DIRECTIVE;
                    token.lexeme          = lexeme;
                    Directive directive   = getDirectiveType(lexeme);
                    token.value.directive = directive;
                }
            }
        }
        tokens[i] = token;
    }

    return tokens;
}

TokenVector* run_lexer(MemoryArena* arena, FILE* input_file, int* total_count)
{
    TokenVector* token_vector = (TokenVector*) arena_alloc(arena, sizeof(TokenVector));
    token_vector->items       = (Token*) arena_calloc(arena, INITIAL_TOKEN_CAPACITY, sizeof(Token));
    token_vector->capacity    = INITIAL_TOKEN_CAPACITY;

    if (token_vector->items == NULL)
    {
        LOG_ERROR("Failed to allocate memory for tokens");
        *total_count = 0;
        return NULL;
    }
    token_vector->capacity = INITIAL_TOKEN_CAPACITY;
    *total_count           = 0;

    const size_t bufferSize = 1024;
    char         buffer[bufferSize];
    while (fgets(buffer, bufferSize, input_file) != NULL)
    {
        char** line       = NULL;
        int    numLexemes = 0;

        line                  = getLexemesInLine(arena, buffer, &numLexemes);
        Token* tokens_in_line = lexer(arena, line, numLexemes);

        if (tokens_in_line == NULL)
        {
            LOG_ERROR("Lexer error, lexer returned NULL!");
        }
        for (int i = 0; i < numLexemes; i++)
        {
            if (*total_count >= token_vector->capacity)
            {
                token_vector->capacity *= 2;
                token_vector->items =
                    (Token*) arena_realloc(arena, token_vector->items, token_vector->capacity);
                if (token_vector->items == NULL)
                {
                    LOG_ERROR("Failed to allocate memory for tokens");
                    *total_count = 0;
                    return NULL;
                }
            }

            token_vector->items[*total_count] = tokens_in_line[i];
            *total_count += 1;
        }
    }
    if (*total_count >= token_vector->capacity)
    {
        token_vector->capacity++;
        token_vector->items =
            (Token*) arena_realloc(arena, token_vector->items, token_vector->capacity);
        if (token_vector->items == NULL)
        {
            LOG_ERROR("Failed to allocate memory for tokens");
            *total_count = 0;
            return NULL;
        }
    }
    Token token_eof                   = {0};
    token_eof.kind                    = TOK_EOF;
    token_eof.lexeme                  = arena_strdup(arena, "EOF");
    token_vector->items[*total_count] = token_eof;
    *total_count += 1;
    return token_vector;
}
