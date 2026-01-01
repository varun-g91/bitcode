#pragma once
#include "arena_allocator.h"
#include "token_stream.h"
#include "vm.h"

extern VMContext*  vm_ctx;
extern MemoryArena lexer_arena;
extern MemoryArena test_parser_arena;

TokenStream* lex_from_string(MemoryArena* arena, const char* source);
