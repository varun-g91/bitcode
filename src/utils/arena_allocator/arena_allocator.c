#include "arena_allocator.h"
#include "logger.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE_FOR_NEXT_ALLOCATIONS 1 * (1024 * 1024) // 1 MB

void arena_init(MemoryArena* arena, size_t initial_size) {
    arena->buffer = malloc(initial_size);
    if (arena->buffer == NULL) {
        LOG_ERROR("Memory allocation failed\n");
        return;
    }
    arena->size   = initial_size;
    arena->offset = 0;
}

void* arena_alloc(MemoryArena* arena, size_t needed_size) {
    size_t total    = needed_size + sizeof(ArenaHeader);
    size_t required = arena->offset + total;

    if (required > arena->size) {
        arena_resize(arena, (required - arena->size) + BUFFER_SIZE_FOR_NEXT_ALLOCATIONS);
    }

    ArenaHeader* h = (ArenaHeader*)(arena->buffer + arena->offset);
    h->size        = needed_size;

    arena->offset += total;

    return (void*)(h + 1); // User pointer follows header
}

void* arena_calloc(MemoryArena* arena, int count, size_t size) {
    size_t total_size;
    if (count == 0 || size == 0) {
        LOG_ERROR("count or size cannot be 0\n");
        return NULL;
    }
    if (size > SIZE_MAX / count) {
        LOG_ERROR("Overflow detected, cannot perform arena_calloc\nABORTING...\n");
        return NULL;
    }
    total_size = count * size;
    void* ptr  = arena_alloc(arena, count * size);
    if (ptr == NULL) {
        LOG_ERROR("Arena allocation failed\n");
        return NULL;
    }
    memset(ptr, 0, total_size);
    return ptr;
}

void arena_resize(MemoryArena* arena, size_t size) {
    if (size == 0) {
        LOG_ERROR("Resize increment cannot be 0.\n");
        return;
    }
    size_t new_size   = arena->size + size;
    char*  new_buffer = (char*)realloc(arena->buffer, new_size);
    if (new_buffer == NULL) {
        LOG_ERROR("Failed to resize arena buffer from %zu to %zu bytes.\nABORTING...\n",
                  arena->size, new_size);
        free(arena->buffer);
        arena->buffer = NULL;
        arena->size   = 0;
        exit(1);
        return;
    }
    arena->size   = new_size;
    arena->buffer = new_buffer;
}

void* arena_realloc(MemoryArena* arena, void* original_ptr, size_t new_size) {
    if (!original_ptr)
        return arena_alloc(arena, new_size);

    ArenaHeader* h        = (ArenaHeader*)original_ptr - 1;
    const size_t old_size = h->size;

    char* block_start = (char*)h;
    char* block_end   = block_start + sizeof(ArenaHeader) + old_size;
    char* arena_end   = arena->buffer + arena->offset;
    if (block_end == arena_end) {
        size_t increment    = new_size - old_size;
        size_t total_needed = arena->offset + increment;

        if (total_needed > arena->size) {
            arena_resize(arena, (total_needed - arena->size) + BUFFER_SIZE_FOR_NEXT_ALLOCATIONS);
        }

        arena->offset += increment;
        h->size = new_size;
        return original_ptr;
    }
    void* new_ptr = arena_alloc(arena, new_size);
    memcpy(new_ptr, original_ptr, old_size);
    return new_ptr;
}

void arena_free(MemoryArena* arena) {
    free(arena->buffer);
    arena->offset = 0;
    arena->size   = 0;
}

char* arena_strdup(MemoryArena* arena, const char* s) {
    const int len  = strlen(s) + 1;
    char*     copy = (char*)arena_alloc(arena, len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}
