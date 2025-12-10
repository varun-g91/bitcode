#include "arena_allocator.h"
#include "unity.h"
#include <stdint.h>

// Assuming your header file for lexer is in include/lexer/lexer.h
// Assuming your VM types (Token, TokenKind, Opcode) are accessible via include/vm/vm_types.h
#define MAX_ARENA_SIZE (1 * 1024 * 1024) // 1 MB
MemoryArena test_suite_arena;

#include "lexer.h" // Replace with your actual lexer header
const uint8_t mov_id    = 0x00;
const uint8_t reg_r1_id = 0x01;
const uint8_t reg_r2_id = 0x02;

void setUp(void) { arena_init(&test_suite_arena, MAX_ARENA_SIZE); }

void tearDown(void) { arena_free(&test_suite_arena); }

// Forward declarations for external test runners
void run_all_lexer_tests(void);

// Helper function to mock fetch_instruction if needed, but for lexer we test tokens
// We'll assume a global array of OpcodeInfo exists for comparison, or use raw values.

void test_lexer_opcode_and_register_tokens(void)
{

    // Test case: MOV R1, R2
    char* input[] = {"MOV", "R1", ",", "R2"};
    int   count   = 4;

    // Assuming your lexer returns a pointer to the start of the token stream
    Token* tokens = lexer(&test_suite_arena, input, count);

    // 1. Check MOV (Opcode)
    TEST_ASSERT_EQUAL_INT(TOK_OPCODE, tokens[0].kind);
    // You must know the numerical value of OP_MOV (e.g., 0x01)
    TEST_ASSERT_EQUAL_HEX(mov_id, tokens[0].value.opcode);

    // 2. Check R1 (Register)
    TEST_ASSERT_EQUAL_INT(TOK_REGISTER, tokens[1].kind);
    // You must know the numerical value of REG_R1 (e.g., 0x01)
    TEST_ASSERT_EQUAL_HEX(reg_r1_id, tokens[1].value.operand.value.reg);

    // 3. Check , (Separator)
    TEST_ASSERT_EQUAL_INT(TOK_SEPARATOR, tokens[2].kind);

    // 4. Check R2 (Register)
    TEST_ASSERT_EQUAL_INT(TOK_REGISTER, tokens[3].kind);
    TEST_ASSERT_EQUAL_HEX(reg_r2_id, tokens[3].value.operand.value.reg);
}

void test_lexer_immediate_and_symbol_tokens(void)
{

    // Test case: JMP 0x12345678
    char* input[] = {"JMP", "0x12345678"};
    int   count   = 2;

    Token* tokens = lexer(&test_suite_arena, input, count);

    // 1. Check JMP
    TEST_ASSERT_EQUAL_INT(TOK_OPCODE, tokens[0].kind);

    // 2. Check 0x12345678 (Literal)
    TEST_ASSERT_EQUAL_INT(TOK_LITERAL, tokens[1].kind);
    // Assuming the lexer stores the value as a 32-bit integer in the token struct
    TEST_ASSERT_EQUAL_HEX32(0x12345678, tokens[1].value.operand.value.literal.value.longValue);
}

// Suite runner function
void run_all_lexer_tests(void)
{
    RUN_TEST(test_lexer_opcode_and_register_tokens);
    RUN_TEST(test_lexer_immediate_and_symbol_tokens);
}
