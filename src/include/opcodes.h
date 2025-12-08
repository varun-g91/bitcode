
#ifndef OPCODES_H
#define OPCODES_H

#include "lexer.h"
#include <stdint.h>

static void init_opcode_table();
Opcode      opcode_lookup(uint8_t);

#endif
