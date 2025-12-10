#include "symbol_table.h"
#include "arena_allocator.h"
#include "uthash.h"
#include <stdbool.h>
#include <stdint.h>

SymbolTable* symbol_table_init() {
    SymbolTable* symbol_table = NULL;
    return symbol_table;
}

bool symbol_table_add(MemoryArena* arena, SymbolTable** symbol_table, const char* key,
                      uint32_t value) {
    SymbolTable* st = NULL;

    HASH_FIND_STR(*symbol_table, key, st);
    if (st != NULL) {
        return false;
    }

    st          = arena_alloc(arena, sizeof(SymbolTable));
    st->symbol  = arena_strdup(arena, key);
    st->address = value;

    HASH_ADD_STR(*symbol_table, symbol, st);

    return true;
}

uint32_t symbol_table_get(SymbolTable* symbol_table, const char* symbol) {
    SymbolTable* st = NULL;
    HASH_FIND_STR(symbol_table, symbol, st);
    return st->address;
}
