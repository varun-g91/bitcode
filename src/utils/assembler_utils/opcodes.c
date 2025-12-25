#include "opcodes.h"
#include "lexer.h"
#include "parser.h"
#include <string.h>

const OpcodeEntry opcode_table[256] = {
    {"PRINT_CHR", 0x00}, {"PRINT_STR", 0x01}, {"MOV", 0x02}, {"LOAD_ADDR", 0x03}, {"ADD", 0x04},
    {"SUB", 0x05},       {"MUL", 0x06},       {"DIV", 0x07}, {"MOD", 0x08},       {"AND", 0x09},
    {"OR", 0x0a},        {"NOT", 0x0b},       {"CMP", 0x0c}, {"JZ", 0x0d},        {"JNZ", 0x0e},
    {"JEQ", 0x0f},       {"JGT", 0x10},       {"JGE", 0x11}, {"JLT", 0x12},       {"JLE", 0x13},
    {"JMP", 0x14},       {"CALL", 0x15},      {"RET", 0x16}, {"PUSH", 0x17},      {"POP", 0x18},
    {"HALT", 0x19}};

Opcode opcode_lookup(const char* s)
{
    for (int i = 0; i < NUM_OPCODES; i++)
    {
        if (strcmp(opcode_table[i].opcode_name, s) == 0)
        {
            return opcode_table[i].opcode_id;
        }
    }
    return 0xFF;
}
