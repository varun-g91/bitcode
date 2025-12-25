#ifndef LEXER_H
#define LEXER_H

#include "arena_allocator.h"
#include <stdbool.h>
#include <stdio.h>

// ------------------------
// TOKEN DEFINITIONS
// ------------------------

// Token categories
typedef enum
{
    TOK_REGISTER,
    TOK_IDENTIFIER,
    TOK_SEPARATOR,
    TOK_DIRECTIVE,
    TOK_UNKNOWN,
    TOK_LITERAL,
    TOK_EOF,
} TokenKind;

// For separators like ',', '\n', etc.
typedef enum
{
    SEP_COMMA,
    SEP_EOL,
} SeparatorKind;

// Opcodes and registers

typedef enum
{
    REG_R0      = 0x00,
    REG_R1      = 0x01,
    REG_R2      = 0x02,
    REG_R3      = 0x03,
    REG_R4      = 0x04,
    REG_R5      = 0x05,
    REG_R6      = 0x06,
    REG_R7      = 0x07,
    REG_SP      = 0x08,
    REG_BP      = 0x09,
    REG_HP      = 0x0a,
    REG_UNKNOWN = 0xFF
} Register;

typedef enum
{
    LIT_INTEGER,
    LIT_CHAR,
    LIT_STRING,
} LiteralType;

typedef union
{
    long  longValue;
    char  charValue;
    char* stringValue;
} LiteralValue;

typedef struct
{
    LiteralType  type;
    LiteralValue value;
} Literal;

typedef union
{
    Register reg;
    Literal  literal;
    char*    symbol;
} OperandValue;

// Types of operand
typedef enum
{
    OT_REGISTER,
    OT_IMMEDIATE_INT,
    OT_IMMEDIATE_CHR,
    OT_IMMEDIATE_STR,
    OT_SYMBOL,
    OT_ANY_SOURCE,
    OT_NONE
} OperandType;

typedef struct
{
    OperandType  type;
    OperandValue value;
} Operand;

// Directives
typedef enum
{
    DIRECTIVE_START  = 0x00,
    DIRECTIVE_DATA   = 0x01,
    DIRECTIVE_RODATA = 0x02,
    DIRECTIVE_GLOBAL = 0x03,
    DIRECT_UNKNOWN   = 0xFF
} Directive;

typedef char* Ientifier;

// Union to hold data for different token kinds
typedef union
{
    Ientifier     identifier;
    Literal       literal;
    Directive     directive;
    SeparatorKind sep;
    Register      reg;
} TokenValue;

// Main token structurestrtol(lexeme, NULL, 10);
typedef struct
{
    TokenKind  kind;
    TokenValue value;
    char*      lexeme;
} Token;
// ------------------------
// OPCODE / REGISTER TABLES
// ------------------------
typedef struct
{
    Token* items;
    int    count;
    int    capacity;
} TokenVector;

extern const char* const OPCODES[];
extern const int         NUM_OPCODES;

extern const char* const REGISTERS[];
extern const int         NUM_REGISTERS;

Register     getRegisterType(const char*);
bool         isRegister(const char*);
char**       getLexemesInLine(MemoryArena*, char*, int*);
Token*       lexer(MemoryArena*, char**, int);
TokenVector* run_lexer(MemoryArena*, FILE*, int*);
#endif
