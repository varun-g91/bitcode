#include "arena_allocator.h"
#include "lexer.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_TOKEN_CAPACITY 256

int main(int argc, char* argv[])
{
    MemoryArena arena;
    arena_init(&arena, MAX_ARENA_SIZE);

    if (argc < 2)
    {
        LOG_ERROR("Usage: %s <input_file>", argv[0]); // Replaced fprintf(stderr, ...)
        return EXIT_FAILURE;
    }

    LOG_INFO("Lexer started with %d arguments.", argc - 1); // Replaced fprintf(stderr, ...)
    char* input_file_path = argv[1];
    LOG_INFO("Input file: %s", input_file_path); // Replaced fprintf(stderr, ...)
    FILE* input_file = fopen(input_file_path, "r");

    int          total_token_count = 0;
    TokenVector* token_vector      = run_lexer(&arena, input_file, &total_token_count);
    if (token_vector->items == NULL)
    {
        LOG_ERROR("Lexing failed.\n");
        fclose(input_file);
        arena_free(&arena);
        return EXIT_FAILURE;
    }
    Token* all_tokens = token_vector->items;
    LOG_INFO("Total tokens produced: %d", total_token_count);
    fprintf(stderr, "--- Starting Token Debug Loop ---\n");
    LOG_DEBUG("total_token_count: %d", total_token_count);
    for (int i = 0; i < total_token_count; i++)
    {
        // Use the debug printing logic you developed
        if (all_tokens[i].kind == TOK_LITERAL)
        {
            LOG_DEBUG("Token #%d value=%lld: Kind=%d, Lexeme=%s", i,
                      all_tokens[i].value.operand.value.literal.value.longValue, all_tokens[i].kind,
                      all_tokens[i].lexeme);
        }
        else
        {
            LOG_DEBUG("Token #%d value=%d: Kind=%d, Lexeme=%s", i, all_tokens[i].value.opcode,
                      all_tokens[i].kind, all_tokens[i].lexeme);
        }
    }

    // TokenStream* stream = build_token_stream(&arena, all_tokens, total_token_count);

    arena_free(&arena);
    fclose(input_file);

    return EXIT_SUCCESS;
}
