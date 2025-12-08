
#ifndef REGISTERS_H
#define REGISTERS_H

#include "lexer.h"
#include <stdint.h>

static void init_register_table();
Register    register_lookup(uint8_t);

#endif
