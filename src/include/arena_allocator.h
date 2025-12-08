#ifndef ARENA_ALLOCATOR_H
#define ARENA_ALLOCATOR_H

#include <stddef.h>
#include <string.h>

typedef struct {
    char*  buffer;
    size_t size;
    size_t offset;
} MemoryArena;

typedef struct {
    size_t size;
} ArenaHeader;

void  arena_init(MemoryArena*, size_t);
void* arena_alloc(MemoryArena*, size_t);
void* arena_calloc(MemoryArena*, int, size_t);
void* arena_realloc(MemoryArena*, void*, size_t);
void  arena_resize(MemoryArena*, size_t);
void  arena_free(MemoryArena*);
char* arena_strdup(MemoryArena*, const char*);
#endif
