#ifndef INSTRUCTION_FORMAT_TABLE_H
#define INSTRUCTION_FORMAT_TABLE_H

#include "lexer.h"

typedef struct {
    const char* name;
    int         operand_count;
    OperandType operands[2];
} OpcodeInfo;

extern const OpcodeInfo opcode_info[256];

#endif
