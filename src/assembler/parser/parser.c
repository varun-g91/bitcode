#include "parser.h"
#include "arena_allocator.h"
#include "assembler_utils.h"
#include "instruction_format_table.h"
#include "lexer.h"
#include "logger.h"
#include "opcodes.h"
#include "token_stream.h"
#include <stdlib.h>
#include <string.h>

int8_t run_parser(MemoryArena* arena, TokenStream* stream, Program* out)
{
    int8_t status = parse_program(stream, arena, out);

    if (status != 0)
    {
        LOG_ERROR("Error while parsing program!");
        return status;
    }

    return 0;
}

int8_t parse_program(TokenStream* stream, MemoryArena* arena, Program* out)
{
    int line_count = 0;
    out->lines     = arena_calloc(arena, out->capcity, sizeof(Line));
    if (out->lines == NULL)
    {
        LOG_ERROR("Error trying to allocate memory for lines");
        return -1;
    }
    if (stream->current == NULL)
    {
    }
    while (stream->current != NULL)
    {
        if (line_count == out->capcity)
        {
            LOG_ERROR("MAX LINE CAPACITY REACHED!");
            exit(1);
        }

        int8_t status = parse_line(arena, stream, &line_count, &out->lines[line_count]);
        if (status != 0)
        {
            LOG_ERROR("Error while parsing line!");
            return status;
        }
    }

    out->count = line_count;

    return 0;
}

int8_t parse_line(MemoryArena* arena, TokenStream* stream, int* line_count, Line* out)
{
    Token token = peek(stream);
    switch (token.kind)
    {
    case TOK_REGISTER:
    {
        break;
    }
    case TOK_SEPARATOR:
    {
        if (token.value.sep == SEP_EOL)
        {
            *line_count += 1;
        }
        consume(stream);
        return 0;
    }
    case TOK_DIRECTIVE:
    {
        ParsedDirective directive;

        int8_t status = parse_directive(stream, arena, &directive);
        if (status != 0)
        {
            LOG_ERROR("Error while parsing directive!");
            return -1;
        }

        out->type            = LINE_DIRECTIVE;
        out->value.directive = directive;
        break;
    }
    case TOK_UNKNOWN:
        break;
    case TOK_IDENTIFIER:
    {
        char* s = token.value.identifier;
        if (s[strlen(s) - 1] == ':')
        {
            int8_t status = parse_label_def(stream, arena, out);
            if (status != 0)
            {
                LOG_ERROR("Error while parsing label_def!");
                return -1;
            }
            line_count++;

            break;
        }
        else
        {
            if (is_opcode(token.value.identifier))
            {
                Instruction instruction;

                int8_t status = parse_instruction(arena, stream, &instruction);
                if (status != 0)
                {
                    LOG_ERROR("Unable to parse instruction");
                    return status;
                }

                out->type              = LINE_INSTRUCTION;
                out->value.instruction = instruction;
                break;
            }
            else
            {
                break;
            }
        }
    }
    case TOK_EOF:
    {
        consume(stream);
        return 0;
    }
    default:
    {
        LOG_ERROR("Expected identifier or directive. Found %s", token.lexeme);
        return -1;
    }
    }
    return 0;
}

int8_t parse_label_def(TokenStream* stream, MemoryArena* arena, Line* out)
{
    Token token = peek(stream);

    size_t len = strlen(token.value.identifier) - 2;
    char*  s   = (char*) arena_alloc(arena, sizeof(char));
    if (s == NULL)
    {
        LOG_ERROR("Failed to allocate memory!");
        return -1;
    }
    strncpy(s, token.value.identifier, len);
    s[len] = '\0';

    out->type        = LINE_LABEL_DEF;
    out->value.label = s;
    consume(stream);
    return 0;
}

int8_t parse_instruction(MemoryArena* arena, TokenStream* stream, Instruction* out)
{
    bool is_valid_operand = peek(stream).kind == TOK_IDENTIFIER ||
                            peek(stream).kind == TOK_LITERAL || peek(stream).kind == TOK_REGISTER;
    Token token = peek(stream);
    if (token.kind == TOK_IDENTIFIER)
    {
        uint8_t opcode_id = 0xFF;
        int8_t  status    = parse_opcode(stream, &opcode_id);
        if (status != 0 || opcode_id == 0xFF)
        {
            LOG_ERROR("Failed to parse opcode");
            return -1;
        }
        out->opcode = (Opcode) opcode_id;

        const OpcodeInfo* info = &opcode_info[opcode_id];

        token = peek(stream);
        if (token.kind == TOK_SEPARATOR)
        {
            consume(stream);
        }

        if (info->operand_count > 0 && is_valid_operand)
        {
            Operand operand_1;
            int8_t  status = parse_operand(stream, arena, &operand_1);
            if (status != 0)
            {
                LOG_ERROR("Error while parsing operand!");
                return -1;
            }

            out->operand_types[0] = operand_1.type;
            out->operands[0]      = operand_1;

            token = peek(stream);
            if (token.kind == TOK_SEPARATOR)
            {
                consume(stream);
            }

            if (info->operand_count > 1 && is_valid_operand)
            {
                Operand operand_2;
                status = parse_operand(stream, arena, &operand_2);
                if (status != 0)
                {
                    LOG_ERROR("Error while parsing operand!");
                    return -1;
                }
                out->operand_types[1] = operand_2.type;
                out->operands[1]      = operand_2;
            }
            else
            {
                out->operand_types[1] = OT_NONE;
            }
        }
        else
        {
            out->operand_types[0] = OT_NONE;
        }
    }
    else
    {
        LOG_ERROR("Identifier expected! Found %s", token.lexeme);
        return -1;
    }
    return 0;
}

int8_t parse_opcode(TokenStream* stream, uint8_t* out)
{
    Token token = peek(stream);
    if (token.kind == TOK_IDENTIFIER)
    {
        const char* opcode_name = token.value.identifier;
        uint8_t     opcode_id   = opcode_lookup(opcode_name);
        if (opcode_id == 0xFF)
        {
            LOG_ERROR("Unknown opcode found!");
            return -1;
        }

        *out = opcode_id;
        consume(stream);
        return 0;
    }
    else
    {
        LOG_ERROR("Opcode expected! Found %s", token.lexeme);
        return -1;
    }
}

int8_t parse_directive(TokenStream* stream, MemoryArena* arena, ParsedDirective* out)
{
    Token token = peek(stream);

    bool is_valid_operand = peek(stream).kind == TOK_IDENTIFIER ||
                            peek(stream).kind == TOK_LITERAL || peek(stream).kind == TOK_REGISTER;
    if (token.kind == TOK_DIRECTIVE)
    {
        out->type = token.value.directive;
        consume(stream);
        if (is_valid_operand)
        {
            Operand operand_1;
            int8_t  status = parse_operand(stream, arena, &operand_1);
            if (status != 0)
            {
                LOG_ERROR("Error while parsing operand!");
                return -1;
            }
            out->operands[0] = operand_1;
            if (is_valid_operand)
            {
                Operand operand_2;
                status = parse_operand(stream, arena, &operand_2);
                if (status != 0)
                {
                    LOG_ERROR("Error while parsing operand!");
                    return -1;
                }
                out->operands[1] = operand_2;
            }
            else
            {
                out->operands[1].type = OT_NONE;
            }
        }
        else
        {
            out->operands[0].type = OT_NONE;
        }
        return 0;
    }
    else
    {
        LOG_ERROR("Directive expected! Found %s", token.lexeme);
        return -1;
    }
}

void parse_comment() {}

int8_t parse_operand(TokenStream* stream, MemoryArena* arena, Operand* out)
{
    if (stream == NULL || stream->current == NULL)
    {
        LOG_ERROR("Current stream or token is null");
        return -1;
    }
    Token token = peek(stream);

    if (token.kind == TOK_SEPARATOR)
    {
        consume(stream);
    }

    token = peek(stream);

    if (token.kind == TOK_REGISTER)
    {
        uint8_t reg_id = parse_register(stream);
        if (reg_id == 0xFF)
        {
            LOG_ERROR("Unknown register found!");
            return -1;
        }
        out->type      = OT_REGISTER;
        out->value.reg = reg_id;
    }
    else if (token.kind == TOK_IDENTIFIER)
    {
        out->type         = OT_SYMBOL;
        out->value.symbol = token.value.identifier;
        consume(stream);
    }
    else
    {
        switch (token.value.literal.type)
        {
        case LIT_INTEGER:
        {
            bool status = parse_integer(stream, (uint32_t*) &out->value.literal.value.longValue);
            if (!status)
            {
                LOG_ERROR("Failed to parse integer literal!");
                return -1;
            }
            out->type = OT_IMMEDIATE_INT;
            break;
        }
        case LIT_CHAR:
        {
            char   chr;
            int8_t status = parse_char(stream, &chr);
            if (status != 0)
            {
                LOG_ERROR("Error parsing character literal!");
                return -1;
            }

            if (!chr)
            {
                LOG_ERROR("Failed to parse character literal!");
                return -1;
            }
            out->type                          = OT_IMMEDIATE_CHR;
            out->value.literal.value.charValue = chr;
            break;
        }
        case LIT_STRING:
        {
            char* str = parse_string(stream, arena);
            if (str == NULL)
            {
                LOG_ERROR("Failed to parse string literal!");
                return -1;
            }
            out->type                            = OT_IMMEDIATE_STR;
            out->value.literal.value.stringValue = str;
            break;
        }
        default:
            LOG_ERROR("Unknown operand type");
            return -1;
        }
    }
    return 0;
}

bool parse_integer(TokenStream* stream, uint32_t* out)
{
    if (stream == NULL || stream->current == NULL)
    {
        LOG_ERROR("Current stream or token is null");
        return false;
    }
    Token token = peek(stream);
    long  val   = token.value.literal.value.longValue;
    if (val < INT32_MIN || val > UINT32_MAX)
    {
        return false;
    }
    *out = (uint32_t) val;
    consume(stream);
    return true;
}

bool parse_address(TokenStream* stream, uint32_t* out)
{
    if (stream == NULL || stream->current == NULL)
    {
        LOG_ERROR("Current stream or token is null");
        return false;
    }
    Token token = peek(stream);
    *out        = token.value.literal.value.longValue;
    if (*out > UINT32_MAX)
    {
        LOG_ERROR("Address literal overflow!\n");
        return false;
    }
    consume(stream);
    return true;
}

int8_t parse_char(TokenStream* stream, char* out)
{
    if (stream == NULL || stream->current == NULL)
    {
        LOG_ERROR("Current stream or token is null");
        return -1;
    }

    Token token        = peek(stream);
    char  char_literal = token.value.literal.value.charValue;
    *out               = char_literal;
    return 0;
}

char* parse_string(TokenStream* stream, MemoryArena* arena)
{
    if (stream == NULL || stream->current == NULL)
    {
        LOG_ERROR("Current stream or token is null");
        return NULL;
    }
    Token token = peek(stream);

    char* s = arena_strdup(arena, token.value.literal.value.stringValue);
    if (!s)
    {
        LOG_ERROR("Memory allocation failed while parsing string literal.");
        return NULL;
    }
    consume(stream);
    return s;
}

uint8_t parse_register(TokenStream* stream)
{
    if (stream == NULL || stream->current == NULL)
    {
        LOG_ERROR("Current stream or token is null");
        return 0xFF;
    }
    Token   current_token = peek(stream);
    uint8_t reg_id;
    if (current_token.kind == TOK_REGISTER)
    {
        reg_id = current_token.value.reg;
        consume(stream);
        return reg_id;
    }
    else
    {
        return 0xFF;
    }
}
