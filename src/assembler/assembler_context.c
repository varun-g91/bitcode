#include "assembler_context.h"
#include "arena_allocator.h"
#include "vm.h"

AssemblerContext* asm_ctx_init(MemoryArena* arena) {
    AssemblerContext* asm_ctx = (AssemblerContext*)arena_alloc(arena, sizeof(AssemblerContext));
    // LOCATION COUNTER
    asm_ctx->location_counter = 0;

    // SECTIONS
    asm_ctx->code_start   = CODE_START;
    asm_ctx->data_start   = DATA_START;
    asm_ctx->rodata_start = RODATA_START;
    asm_ctx->initial_hp   = HEAP_START;
    asm_ctx->initial_sp   = STACK_START;

    // SYMBOL TABLE
    asm_ctx->symbol_table       = NULL;
    asm_ctx->instruction_vector = (InstructionVector*)arena_alloc(arena, sizeof(InstructionVector));

    // MEMORY
    asm_ctx->memory = (uint8_t*)arena_alloc(arena, MEM_SIZE);

    return asm_ctx;
}
