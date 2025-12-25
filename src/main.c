#include "arena_allocator.h"
#include "lexer.h"
#include "logger.h"
#include "parser.h"
#include "token_stream.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_TOKEN_CAPACITY 256

int main(int argc, char* argv[])
{

    if (argc < 4)
    {
        LOG_ERROR("usage: %s [global options] <command> [command options] [arguments]\n",
                  argv[0]); // Replaced fprintf(stderr, ...)
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "vm") == 0)
    {
        if (strcmp(argv[2], "run") == 0)
        {
            VMContext* ctx             = vm_create();
            char*      input_file_path = argv[3];
            LOG_INFO("Input file: %s", input_file_path); // Replaced fprintf(stderr, ...)
            int8_t status = run_vm(ctx, argv[3]);
            if (status != VM_EXIT_SUCCESS)
            {
                LOG_ERROR("VM exited with error code %d\n", status);
                vm_destroy(ctx);
                return status;
            }
            else
            {
                LOG_INFO("VM finished execution successfully.\n", VM_EXIT_SUCCESS);
                vm_destroy(ctx);
                return VM_EXIT_SUCCESS;
            }
        }
    }
    int          total_token_count;
    TokenStream* stream;
    if (strcmp(argv[1], "asm") == 0)
    {
        if (strcmp(argv[2], "run") == 0)
        {
            MemoryArena arena;
            arena_init(&arena, 2 * MAX_ARENA_SIZE);

            LOG_INFO("Lexer started with %d arguments.", argc - 1); // Replaced fprintf(stderr, ...)
            char* input_file_path = argv[3];
            LOG_INFO("Input file: %s", input_file_path); // Replaced fprintf(stderr, ...)
            FILE* input_file = fopen(input_file_path, "r");

            total_token_count         = 0;
            TokenVector* token_vector = run_lexer(&arena, input_file, &total_token_count);
            if (token_vector->items == NULL)
            {
                LOG_ERROR("Lexing failed.\n");
                fclose(input_file);
                arena_free(&arena);
                return EXIT_FAILURE;
            }
            Token* all_tokens = token_vector->items;
            LOG_INFO("Total tokens produced: %d", total_token_count);

            stream = build_token_stream(&arena, all_tokens, total_token_count);

            fclose(input_file);

            Program program = {0};
            program.capcity = INITITAL_LINE_CAPACITY;

            int8_t status = run_parser(&arena, stream, &program);
            if (status != 0)
            {
                LOG_ERROR("Error while running parser: %d");
                return status;
            }

            arena_free(&arena);
        }
    }

    return EXIT_SUCCESS;
}
