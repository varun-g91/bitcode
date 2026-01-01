#include "unity_internals.h"
#include <stdio.h>

// External function declarations for the test suites
void run_all_lexer_tests(void);
void run_all_parser_tests(void);
void run_all_decoder_tests(void);
void run_all_vm_tests(void);

// void setUp(void) { ctx = vm_create(); }
// void tearDown(void) { vm_destroy(ctx); }
// void setUp(void) { arena_init(&test_suite_arena, MAX_ARENA_SIZE); }
//
// void tearDown(void) { arena_free(&test_suite_arena); }
/**
 * @brief Main function for the test executable.
 *
 * It initializes Unity and runs the test suites.
 */
int main(void)
{
    printf("--- Starting BitLang VM Tests ---\n");

    UNITY_BEGIN();

    // Call the functions that contain the RUN_TEST() calls for each suite
    run_all_lexer_tests();
    run_all_parser_tests();
    run_all_decoder_tests();
    run_all_vm_tests();

    return UNITY_END();
}
