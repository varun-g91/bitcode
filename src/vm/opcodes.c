#include "opcodes.h"
#include "lexer.h"

static Opcode opcode_from_value[256];

static bool opcode_table_initialized = false;

static void init_opcode_table() {
    // Default everything to OP_UNKNOWN
    for (int i = 0; i < 256; i++)
        opcode_from_value[i] = OP_UNKNOWN;

    // Now fill real ones
    opcode_from_value[0x00] = OP_STORE;
    opcode_from_value[0x01] = OP_LOAD;
    opcode_from_value[0x02] = OP_ADD;
    opcode_from_value[0x03] = OP_SUB;
    opcode_from_value[0x04] = OP_MUL;
    opcode_from_value[0x05] = OP_DIV;
    opcode_from_value[0x06] = OP_MOD;
    opcode_from_value[0x07] = OP_AND;
    opcode_from_value[0x08] = OP_OR;
    opcode_from_value[0x09] = OP_NOT;
    opcode_from_value[0x0A] = OP_CMP;

    opcode_from_value[0x0B] = OP_JZ;
    opcode_from_value[0x0C] = OP_JNZ;
    opcode_from_value[0x0D] = OP_JEQ;
    opcode_from_value[0x0E] = OP_JGT;
    opcode_from_value[0x0F] = OP_JGE;
    opcode_from_value[0x10] = OP_JLT;
    opcode_from_value[0x11] = OP_JLE;

    opcode_from_value[0x12] = OP_CALL;
    opcode_from_value[0x13] = OP_RET;
    opcode_from_value[0x14] = OP_JMP;

    opcode_from_value[0x15] = OP_PUSH;
    opcode_from_value[0x16] = OP_POP;

    opcode_table_initialized = true;
}

Opcode opcode_lookup(uint8_t byte) {
    if (!opcode_table_initialized) {
        init_opcode_table();
    }
    return opcode_from_value[byte];
}
