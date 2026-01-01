
#ifndef OPCODES_H
#define OPCODES_H
#include "parser.h"
#include <stdint.h>

typedef struct
{
    char*   opcode_name;
    uint8_t opcode_id;
} OpcodeEntry;

extern const OpcodeEntry opcode_table[256];

Opcode opcode_lookup(const char*);
char*  ident_lookup(uint8_t opcode_id);

#endif
