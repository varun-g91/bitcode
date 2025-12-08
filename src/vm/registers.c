#include "registers.h"
#include "lexer.h"
#include <stdint.h>

static Register reg_from_value[256];

static bool reg_table_initialized = false;

void init_register_table() {
    for (int i = 0; i < NUM_REGISTERS; i++) {
        reg_from_value[i] = REG_UNKNOWN;
    }

    reg_from_value[0x00] = REG_R0;
    reg_from_value[0x01] = REG_R1;
    reg_from_value[0x02] = REG_R2;
    reg_from_value[0x03] = REG_R3;
    reg_from_value[0x04] = REG_R4;
    reg_from_value[0x05] = REG_R5;
    reg_from_value[0x06] = REG_R6;
    reg_from_value[0x07] = REG_R7;
    reg_from_value[0x08] = REG_SP;
    reg_from_value[0x09] = REG_BP;
    reg_from_value[0xFF] = REG_UNKNOWN;

    reg_table_initialized = true;
}

Register register_lookup(uint8_t byte) {
    if (!reg_table_initialized) {
        init_register_table();
    }
    return reg_from_value[byte];
}
