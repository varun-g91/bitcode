#include "instruction_format_table.h"

#include "parser.h"

const OpcodeInfo opcode_info[256] = {
    // Opcode        | .name      | .operand_count | .operands[0] (Dest/Jump) | .operands[1]
    // (Source) | .operands[2]

    // --- Utility ---
    [OP_PRINT_CHR] = {"print_chr", 1, {OT_REGISTER, OT_NONE}},
    [OP_PRINT_STR] = {"print_str", 1, {OT_ANY_SOURCE, OT_NONE}},

    // --- Data Transfer ---
    [OP_MOV]       = {"mov", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_LOAD_ADDR] = {"load_addr", 2, {OT_REGISTER, OT_ANY_SOURCE}},

    // --- Arithmetic / Logic ---
    [OP_ADD] = {"add", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_SUB] = {"sub", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_MUL] = {"mul", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_DIV] = {"div", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_MOD] = {"mod", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_AND] = {"and", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_OR]  = {"or", 2, {OT_REGISTER, OT_ANY_SOURCE}},
    [OP_NOT] = {"not", 1, {OT_REGISTER, OT_NONE}},
    [OP_CMP] = {"cmp", 0, {OT_NONE, OT_NONE}},

    // --- Control Flow (All jumps/calls target a label/symbol) ---
    [OP_JZ]   = {"jz", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JNZ]  = {"jnz", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JEQ]  = {"jeq", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JGT]  = {"jgt", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JGE]  = {"jge", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JLT]  = {"jlt", 1, {OT_SYMBOL, OT_NONE}},
    [OP_JLE]  = {"jle", 1, {OT_SYMBOL, OT_NONE}},
    [OP_CALL] = {"call", 1, {OT_SYMBOL, OT_NONE}},
    [OP_RET]  = {"ret", 0, {OT_NONE, OT_NONE}},
    [OP_JMP]  = {"jmp", 1, {OT_SYMBOL, OT_NONE}},

    // --- Stack Operations ---
    [OP_PUSH] = {"push", 1, {OT_REGISTER, OT_NONE}},
    [OP_POP]  = {"pop", 1, {OT_REGISTER, OT_NONE}},

    [OP_HALT] = {"halt", 0, {OT_NONE, OT_NONE}},

    // --- Unknown ---
    [OP_UNKNOWN] = {"unknown", 0, {OT_NONE, OT_NONE}}};
