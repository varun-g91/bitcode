#include "opcodes.h"
#include "lexer.h"
#include "logger.h"
#include "parser.h"
#include <ctype.h>
#include <string.h>

const OpcodeEntry opcode_table[256] = {
    {"print_chr", 0x00}, {"print_str", 0x01}, {"mov", 0x02}, {"load_addr", 0x03}, {"add", 0x04},
    {"sub", 0x05},       {"mul", 0x06},       {"div", 0x07}, {"mod", 0x08},       {"and", 0x09},
    {"or", 0x0a},        {"not", 0x0b},       {"cmp", 0x0c}, {"jz", 0x0d},        {"jnz", 0x0e},
    {"jeq", 0x0f},       {"jgt", 0x10},       {"jge", 0x11}, {"jlt", 0x12},       {"jle", 0x13},
    {"jmp", 0x14},       {"call", 0x15},      {"ret", 0x16}, {"push", 0x17},      {"pop", 0x18},
    {"halt", 0x19}};

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

char* ident_lookup(uint8_t opcode_id)
{
    for (int i = 0; i < NUM_OPCODES; i++)
    {
        if (opcode_table[i].opcode_id == opcode_id)
        {
            return opcode_table[i].opcode_name;
        }
    }
    return NULL;
}
