#include "test_common.h"
#include "arena_allocator.h"
#include "lexer.h"
#include "token_stream.h"
#include "unity.h"
#include "vm.h"
#include <stdio.h>

VMContext*  vm_ctx;
MemoryArena lexer_arena;
MemoryArena test_parser_arena;

void setUp()
{
    vm_ctx = vm_create();
    arena_init(&lexer_arena, MAX_ARENA_SIZE);
    arena_init(&test_parser_arena, MAX_ARENA_SIZE);
}
void tearDown()
{
    vm_destroy(vm_ctx);
    arena_free(&lexer_arena);
    arena_free(&test_parser_arena);
}

TokenStream* lex_from_string(MemoryArena* arena, const char* source)
{
    FILE* f = tmpfile();
    if (!f)
        return NULL;

    fputs(source, f);
    rewind(f);

    int          token_count = 0;
    TokenVector* tokens      = run_lexer(arena, f, &token_count);
    fclose(f);

    if (!tokens)
        return NULL;
    return build_token_stream(arena, tokens->items, token_count);
}
