#ifndef ASSEMBLER_CONTEXT_H
#define ASSEMBLER_CONTEXT_H
#include "arena_allocator.h"
#include "parser.h"
#include "symbol_table.h"
#include <stddef.h>
#include <stdint.h>

typedef struct
{
    uint32_t location_counter;
    uint32_t code_start;
    size_t   code_size;

    uint32_t data_start;
    uint32_t rodata_start;

    uint32_t initial_hp;
    uint32_t initial_sp;

    Program       program;
    SymbolTable** symbol_table;

    uint8_t* memory;

} AssemblerContext;

AssemblerContext* asm_init(MemoryArena*);

#endif // !ASSEMBLER_CONTEXT_H
