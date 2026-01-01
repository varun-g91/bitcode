#include "arena_allocator.h"
#include <stdbool.h>
#include <stddef.h>
size_t calculate_vm_data_size(const char*);
bool   is_opcode(const char*, MemoryArena*);
