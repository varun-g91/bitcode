#include "instruction_format_table.h"

const OpcodeInfo opcode_info[256] = {
    // Opcode        | .name      | .operand_count | .operands[0] (Dest/Jump) | .operands[1]
    // (Source) | .operands[2]

    // --- Utility ---
    [OP_PRINT_CHR] = {"PRINT", 1, {OT_REGISTER, OT_NONE}},
    [OP_PRINT_STR] = {"PRINT", 1, {OT_ANY_SOURCE, OT_NONE}},

    // --- Data Transfer ---
    [OP_MOV]       = {"MOV", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_LOAD_ADDR] = {"LOAD_ADDR", 2, {OT_REGISTER, OT_ANY_SOURCE}},

    // --- Arithmetic / Logic ---
    [OP_ADD] = {"ADD", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_SUB] = {"SUB", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_MUL] = {"MUL", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_DIV] = {"DIV", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_MOD] = {"MOD", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_AND] = {"AND", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_OR]  = {"OR", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_NOT] = {"NOT", 1, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_CMP] = {"CMP", 2, {OT_REGISTER, OT_ANY_SOURCE}},

    // --- Control Flow (All jumps/calls target a label/symbol) ---
    [OP_JZ]   = {"JZ", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JNZ]  = {"JNZ", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JEQ]  = {"JEQ", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JGT]  = {"JGT", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JGE]  = {"JGE", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JLT]  = {"JLT", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JLE]  = {"JLE", 1, {OT_SYMBOL, OT_NONE}},
    [OP_CALL] = {"CALL", 1, {OT_SYMBOL, OT_NONE}},
    [OP_RET]  = {"RET", 0, {OT_NONE, OT_NONE}},
    [OP_JMP]  = {"JMP", 1, {OT_SYMBOL, OT_NONE}},

    // --- Stack Operations ---
    [OP_PUSH] = {"PUSH", 1, {OT_REGISTER, OT_NONE}},
    [OP_POP]  = {"POP", 1, {OT_REGISTER, OT_NONE}},

    // --- Unknown ---
    [OP_UNKNOWN] = {"UNKNOWN", 0, {OT_NONE, OT_NONE}}};
