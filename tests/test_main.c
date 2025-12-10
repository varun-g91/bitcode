#include "unity.h"
#include <stdio.h>

// External function declarations for the test suites
void run_all_lexer_tests(void);
void run_all_decoder_tests(void);

/**
 * @brief Main function for the test executable.
 *
 * It initializes Unity and runs the test suites.
 */
int main(void) {
    printf("--- Starting BitLang VM Tests ---\n");

    UNITY_BEGIN();

    // Call the functions that contain the RUN_TEST() calls for each suite
    run_all_lexer_tests();
    // run_all_decoder_tests();

    return UNITY_END();
}
