#ifndef LEXER_H
#define LEXER_H

#include "arena_allocator.h"
#include <stdbool.h>
#include <stdio.h>

// ------------------------
// TOKEN DEFINITIONS
// ------------------------

// Token categories
typedef enum {
    TOK_OPCODE,
    TOK_REGISTER,
    TOK_NUMBER,
    TOK_IDENTIFIER,
    TOK_LABEL_DEF,
    TOK_SYMBOL,
    TOK_SEPARATOR,
    TOK_DIRECTIVE,
    TOK_UNKNOWN,
    TOK_LITERAL,
} TokenKind;

// For separators like ',', '\n', etc.
typedef enum {
    SEP_COMMA,
    SEP_EOL,
    SEP_EOF,
} SeparatorKind;

// Opcodes and registers

typedef enum {
    OP_PRINT = 0x00,
    OP_MOV   = 0x01,
    OP_ADD   = 0x02,
    OP_SUB   = 0x03,
    OP_MUL   = 0x04,
    OP_DIV   = 0x05,
    OP_MOD   = 0x06,
    OP_AND   = 0x07,
    OP_OR    = 0x08,
    OP_NOT   = 0x09,
    OP_CMP   = 0x0A,

    OP_JZ  = 0x0B,
    OP_JNZ = 0x0C,
    OP_JEQ = 0x0D,
    OP_JGT = 0x0E,
    OP_JGE = 0x0F,
    OP_JLT = 0x10,
    OP_JLE = 0x11,

    OP_CALL = 0x12,
    OP_RET  = 0x13,
    OP_JMP  = 0x14,

    OP_PUSH = 0x15,
    OP_POP  = 0x16,

    OP_UNKNOWN = 0xFF
} Opcode;

typedef enum {
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
    REG_UNKNOWN = 0xFF
} Register;

typedef enum {
    LIT_LONG_LONG,
    LIT_CHAR,
    LIT_STR,
} LiteralType;

typedef union {
    long long longValue;
    char      charValue;
    char*     stringValue;
} LiteralValue;

typedef struct {
    LiteralType  type;
    LiteralValue value;
} Literal;

typedef union {
    Register reg;
    Literal  literal;
    char*    symbol;
} OperandValue;

// Types of operand
typedef enum { OT_REGISTER, OT_IMMEDIATE, OT_SYMBOL, OT_ANY_SOURCE, OT_NONE } OperandType;

typedef struct {
    OperandType  type;
    OperandValue value;
} Operand;

// Directives
typedef enum { DIRECT_START, DIRECT_DATA, DIRECT_RODATA, DIRECT_UNKNOWN } Directive;

// Union to hold data for different token kinds
typedef union {
    Opcode        opcode;
    Operand       operand;
    Directive     directive;
    char*         label_def;
    SeparatorKind sep;
} TokenValue;

// Main token structurestrtol(lexeme, NULL, 10);
typedef struct {
    TokenKind  kind;
    TokenValue value;
    char*      lexeme; // original text, optional but useful for debugging
} Token;
// ------------------------
// OPCODE / REGISTER TABLES
// ------------------------
typedef struct {
    Token* items;
    int    count;
    int    capacity;
} TokenVector;

extern const char* const OPCODES[];
extern const int         NUM_OPCODES;

extern const char* const REGISTERS[];
extern const int         NUM_REGISTERS;

Opcode       getOpcodeType(const char*);
Register     getRegisterType(const char*);
bool         isOpcode(const char*);
bool         isRegister(const char*);
char**       getLexemesInLine(MemoryArena*, char*, int*);
Token*       lexer(MemoryArena*, char**, int);
TokenVector* run_lexer(MemoryArena*, FILE*, int*);
#endif
