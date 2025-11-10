#include "logger.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Const memory
#define MEMORY_SIZE 1024
// Logging
int memory[MEMORY_SIZE];
int stack[MEMORY_SIZE];

const char* opcodes[]   = {"STORE", "LOAD", "ADD", "SUB", "MUL", "DIV", "MOD", "AND", "OR", "NOT"};
const int   NUM_OPCODES = sizeof(opcodes) / sizeof(opcodes[0]);

const char* registers[]   = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "SP", "BP"};
const int   NUM_REGISTERS = sizeof(registers) / sizeof(registers[0]);

typedef enum { STORE, LOAD, ADD, SUB, MUL, DIV, MOD, AND, OR, NOT } TypeOpcode;

typedef enum { R0, R1, R2, R3, R4, R5, R6, R7, SP, BP } TypeRegister;

typedef enum { INTEGER_LETERAL, CHAR_LITERAL, STRING_LITERAL } TypeLiteral;

typedef enum { TYPE_OPCODE, TYPE_REGISTER, TYPE_LITERAL, TYPE_SEPARATOR } TypeTokenKind;
typedef enum { COMMA, END_OF_LINE } TypeSeparator;

typedef union {
    TypeOpcode    opcode;
    TypeRegister  reg;
    TypeSeparator separator;
    int           intValue;
    char          charValue;
    char*         strValue;
} TypeToken;

typedef struct {
    TypeTokenKind kind;
    TypeToken     name;
    TypeSeparator separator;
    char*         lexeme;
} Token;

TypeOpcode getOpcodeType(char* lexeme) {
    if (strcmp(lexeme, "STORE") == 0)
        return STORE;
    if (strcmp(lexeme, "LOAD") == 0)
        return LOAD;
    if (strcmp(lexeme, "ADD") == 0)
        return ADD;
    if (strcmp(lexeme, "SUB") == 0)
        return SUB;
    if (strcmp(lexeme, "MUL") == 0)
        return MUL;
    if (strcmp(lexeme, "DIV") == 0)
        return DIV;
    if (strcmp(lexeme, "MOD") == 0)
        return MOD;
    return -1; // Unknown opcode
}

TypeRegister getRegisterType(char* lexeme) {
    if (strcmp(lexeme, "R0") == 0)
        return R0;
    if (strcmp(lexeme, "R1") == 0)
        return R1;
    if (strcmp(lexeme, "R2") == 0)
        return R2;
    if (strcmp(lexeme, "R3") == 0)
        return R3;
    if (strcmp(lexeme, "R4") == 0)
        return R4;
    if (strcmp(lexeme, "R5") == 0)
        return R5;
    if (strcmp(lexeme, "R6") == 0)
        return R6;
    if (strcmp(lexeme, "R7") == 0)
        return R7;
    if (strcmp(lexeme, "SP") == 0)
        return SP;
    if (strcmp(lexeme, "BP") == 0)
        return BP;
    return -1; // Unknown register
}

bool isOpcode(const char* s) {
    for (int i = 0; i < NUM_OPCODES; i++) {
        if (strcmp(s, opcodes[i]) == 0)
            return true;
    }
    return false;
}

bool isRegister(const char* s) {
    for (int i = 0; i < NUM_REGISTERS; i++) {
        if (strcmp(s, registers[i]) == 0) {
            return true;
        }
    }
    return false;
}

char** getLexemesInLine(char* line, int* count) {
    char** lexemes = (char**)malloc(100 * sizeof(char*));
    *count         = 0;
    // line = "LOAD R1, 100\0"
    char* lineCopy = line;
    while (*lineCopy) {
        while (isspace((unsigned char)*lineCopy)) {
            lineCopy++;
        }
        if (*lineCopy == '\0' || *lineCopy == '\n' || *lineCopy == '\r')
            break;

        char lexeme[100];
        int  j = 0;
        if (*lineCopy == ',') {
            lexeme[0] = *lineCopy;
            j         = 1;
            lineCopy++;
        } else {
            while (*lineCopy && !isspace((unsigned char)*lineCopy) && *lineCopy != ',' &&
                   *lineCopy != '\n' && *lineCopy != '\r') {
                lexeme[j++] = *lineCopy++;
            }
        }

        if (j > 0) {
            lexeme[j]       = '\0';
            lexemes[*count] = (char*)malloc(strlen(lexeme) + 1);
            strcpy(lexemes[*count], lexeme);
            (*count)++;
        }
    }
    return lexemes;
}

Token* lexer(char** line, int count) {
    Token* tokens = (Token*)malloc(count * sizeof(Token));
    for (int i = 0; i < count; i++) {
        Token token  = {0};
        char* lexeme = line[i];
        if (*lexeme) {
            LOG_DEBUG("Processing lexeme: %s", lexeme); // Replaced printf

            if (isOpcode(lexeme)) {
                token.kind        = TYPE_OPCODE;
                token.name.opcode = getOpcodeType(lexeme);
                token.lexeme      = lexeme;
            } else if (isRegister(lexeme)) {
                token.kind     = TYPE_REGISTER;
                token.name.reg = getRegisterType(lexeme);
                token.lexeme   = lexeme;
            } else {
                if (isdigit(lexeme[0]) || (lexeme[0] == '-' && isdigit(lexeme[1]))) {
                    token.kind          = TYPE_LITERAL;
                    token.name.intValue = atoi(lexeme);
                    token.lexeme        = lexeme;
                } else if (lexeme[0] == '\'' && lexeme[2] == '\'') {
                    token.kind           = TYPE_LITERAL;
                    token.name.charValue = lexeme[1];
                    token.lexeme         = lexeme;
                } else if (lexeme[0] == '\"' && lexeme[strlen(lexeme) - 1] == '\"') {
                    token.kind          = TYPE_LITERAL;
                    token.name.strValue = (char*)malloc(strlen(lexeme) - 1);
                    strncpy(token.name.strValue, lexeme + 1, strlen(lexeme) - 2);
                    token.name.strValue[strlen(lexeme) - 2] = '\0';
                    token.lexeme                            = lexeme;
                } else if (strcmp(lexeme, ",") == 0) {
                    token.kind           = TYPE_SEPARATOR;
                    token.name.separator = COMMA;
                    token.lexeme         = lexeme;
                } else {
                    LOG_WARN("Unknown lexeme: %s", lexeme); // Replaced printf
                }
            }
        }

        tokens[i] = token;
    }

    return tokens;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        LOG_ERROR("Usage: %s <input_file>", argv[0]); // Replaced fprintf(stderr, ...)
        return EXIT_FAILURE;
    }

    LOG_INFO("Lexer started with %d arguments.", argc - 1); // Replaced fprintf(stderr, ...)
    char* inputFilePath = argv[1];
    LOG_INFO("Input file: %s", inputFilePath); // Replaced fprintf(stderr, ...)

    const size_t bufferSize = 1024;
    char         buffer[bufferSize];
    FILE*        inputFile = fopen(inputFilePath, "r");

    if (inputFile == NULL) {
        LOG_ERROR("Error opening file: Could not open %s.",
                  inputFilePath); // Replaced fprintf(stderr, ...)
        return EXIT_FAILURE;
    }

    int lineNumber = 0;

    while (fgets(buffer, bufferSize, inputFile) != NULL) {
        char** line       = (char**)malloc(100 * sizeof(char*));
        int    numLexemes = 0;

        LOG_DEBUG("Reading line %d: %s", lineNumber, buffer); // Replaced fprintf(stderr, ...)

        line = getLexemesInLine(buffer, &numLexemes);

        Token* tokens = lexer(line, numLexemes);

        for (int i = 0; i < numLexemes; i++) {
            LOG_DEBUG("Token %d: Kind=%d, Lexeme=%s", i, tokens[i].kind, tokens[i].lexeme);
        }

        for (int i = 0; i < numLexemes; i++) {
            free(line[i]);
        }
        free(tokens);
        lineNumber++;
        free(line);
    }

    fclose(inputFile);

    return EXIT_SUCCESS;
}
