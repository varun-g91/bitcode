#include "arena_allocator.h"
#include "logger.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE_FOR_NEXT_ALLOCATIONS 1 * (1024 * 1024) // 1 MB

void arena_init(MemoryArena* arena, size_t initial_size)
{
    arena->buffer = malloc(initial_size);
    if (arena->buffer == NULL)
    {
        LOG_ERROR("Memory allocation failed\n");
        return;
    }
    arena->size   = initial_size;
    arena->offset = 0;
}

void* arena_alloc(MemoryArena* arena, size_t needed_size)
{
    size_t total    = needed_size + sizeof(ArenaHeader);
    size_t required = arena->offset + total;

    if (required > arena->size)
    {
        LOG_ERROR("OUT OF MEMORY! Required: %d; Available: %d\nABORTING...", required, arena->size);
        exit(1);
    }

    ArenaHeader* h = (ArenaHeader*) (arena->buffer + arena->offset);
    h->size        = needed_size;

    arena->offset += total;

    return (void*) (h + 1); // User pointer follows header
}

void* arena_calloc(MemoryArena* arena, int count, size_t size)
{
    size_t total_size;
    if (count == 0 || size == 0)
    {
        LOG_ERROR("count or size cannot be 0\n");
        return NULL;
    }
    if (size > SIZE_MAX / count)
    {
        LOG_ERROR("Overflow detected, cannot perform arena_calloc\nABORTING...\n");
        return NULL;
    }
    total_size = count * size;
    void* ptr  = arena_alloc(arena, count * size);
    if (ptr == NULL)
    {
        LOG_ERROR("Arena allocation failed\n");
        return NULL;
    }
    memset(ptr, 0, total_size);
    return ptr;
}

void* arena_realloc(MemoryArena* arena, void* original_ptr, size_t new_size) // NOLINT:
{
    (void) arena;
    (void) original_ptr;
    (void) new_size;
    LOG_ERROR("arena_realloc is disabled for parser testing\n");
    exit(1);
}

void arena_free(MemoryArena* arena)
{
    free(arena->buffer);
    arena->offset = 0;
    arena->size   = 0;
}

char* arena_strdup(MemoryArena* arena, const char* s)
{
    const size_t len  = strlen(s);
    char*        copy = (char*) arena_alloc(arena, len + 1);
    if (copy)
    {
        memcpy(copy, s, len);
    }
    copy[len] = '\0';
    return copy;
}
