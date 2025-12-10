#include "arena_allocator.h"
#include "uthash.h"
#include <stdbool.h>

typedef struct {
    char*          symbol;
    uint32_t       address;
    UT_hash_handle hh;
} SymbolTable;

SymbolTable* symbol_table_init();
bool         symbol_table_add(MemoryArena*, SymbolTable**, const char*, uint32_t);
uint32_t     symbol_table_lookup(SymbolTable*, const char*);
