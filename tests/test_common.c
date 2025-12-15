#include "test_common.h"
#include "arena_allocator.h"
#include "vm.h"
VMContext*  vm_ctx;
MemoryArena test_suite_arena;

void setUp(void)
{
    vm_ctx = vm_create();

    // Assuming you have a function like arena_create()
    arena_init(&test_suite_arena, MAX_ARENA_SIZE);
}

void tearDown(void)
{
    vm_destroy(vm_ctx);
    // Assuming you have a function like arena_destroy()
    arena_free(&test_suite_arena);
}
