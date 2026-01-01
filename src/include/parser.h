#ifndef PARSER_H
#define PARSER_H

#include "token_stream.h"
#include <stdint.h>
// Const memory
#define MEMORY_SIZE 1024
#define MAX_TOKENS_PER_INSTRUCTION 5
#define INITITAL_LINE_CAPACITY 100
// Logging
//

typedef enum
{
    // I/O
    OP_PRINT_CHR = 0x00,
    OP_PRINT_STR = 0x01,
    // Data transfer
    OP_MOV       = 0x02,
    OP_LOAD_ADDR = 0x03, // LOAD_ADDR R1, my_string;

    // Arithmatic
    OP_ADD = 0x04,
    OP_SUB = 0x05,
    OP_MUL = 0x06,
    OP_DIV = 0x07,
    OP_MOD = 0x08,
    // Logical
    OP_AND = 0x09,
    OP_OR  = 0x0a,
    OP_NOT = 0x0B,
    // Control flow
    OP_CMP = 0x0C,
    OP_JZ  = 0x0D,
    OP_JNZ = 0x0E,
    OP_JEQ = 0x0f,
    OP_JGT = 0x10,
    OP_JGE = 0x11,
    OP_JLT = 0x12,
    OP_JLE = 0x13,
    OP_JMP = 0x14,
    // Procedure
    OP_CALL = 0x15,
    OP_RET  = 0x16,
    // Stack
    OP_PUSH = 0x17,
    OP_POP  = 0x18,
    OP_HALT = 0x19,
    // Unknown
    OP_UNKNOWN = 0xFF
} Opcode;
typedef struct
{
    Opcode      opcode;
    OperandType operand_types[2];
    Operand     operands[2];
} Instruction;

typedef enum
{
    LINE_LABEL_DEF   = 0,
    LINE_INSTRUCTION = 1,
    LINE_DIRECTIVE   = 2,
} LineType;

typedef struct
{
    Directive type;
    Operand   operands[2];
} ParsedDirective;

typedef union
{
    const char*     label;
    Instruction     instruction;
    ParsedDirective directive;
} LineValue;

typedef struct
{
    LineType  type;
    LineValue value;
} Line;

typedef struct
{
    Line* lines;
    int   capcity;
    int   count;
} Program;

int8_t run_parser(MemoryArena*, TokenStream*, Program*);
int8_t parse_program(TokenStream*, MemoryArena*, Program*);

int8_t parse_line(MemoryArena*, TokenStream*, Line*);

int8_t parse_label_def(TokenStream*, MemoryArena*, Line*);

int8_t parse_directive(TokenStream*, MemoryArena*, ParsedDirective*);

int8_t parse_instruction(MemoryArena*, TokenStream*, Instruction*);

int8_t parse_opcode(TokenStream*, uint8_t*, MemoryArena*);

void parse_comment();

void consume_new_line();

int8_t parse_operand(TokenStream*, MemoryArena*, Operand*);
#include "assembler_context.h"

bool parse_integer(TokenStream*, uint32_t*);

bool parse_address(TokenStream*, uint32_t*);

int8_t parse_char(TokenStream*, char*);

char* parse_string(TokenStream*, MemoryArena*);

uint8_t parse_register(TokenStream*);

#endif // !PARSER_H
