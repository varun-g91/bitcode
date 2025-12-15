#include "test_common.h"
#include "unity.h"
#include <stdint.h>

#define MAX_ARENA_SIZE (1 * 1024 * 1024) // 1 MB

#include "lexer.h"
const uint8_t mov_id    = 0x02;
const uint8_t reg_r1_id = 0x01;
const uint8_t reg_r2_id = 0x02;

// void setUp(void) { arena_init(&test_suite_arena, MAX_ARENA_SIZE); }
//
// void tearDown(void) { arena_free(&test_suite_arena); }

void run_all_lexer_tests(void);

void test_lexer_opcode_and_register_tokens(void)
{

    // Test case: MOV R1, R2
    char* input[] = {"MOV", "R1", ",", "R2"};
    int   count   = 4;

    Token* tokens = lexer(&test_suite_arena, input, count);

    TEST_ASSERT_EQUAL_INT(TOK_OPCODE, tokens[0].kind);
    TEST_ASSERT_EQUAL_HEX(mov_id, tokens[0].value.opcode);

    TEST_ASSERT_EQUAL_INT(TOK_REGISTER, tokens[1].kind);
    TEST_ASSERT_EQUAL_HEX(reg_r1_id, tokens[1].value.operand.value.reg);

    TEST_ASSERT_EQUAL_INT(TOK_SEPARATOR, tokens[2].kind);

    TEST_ASSERT_EQUAL_INT(TOK_REGISTER, tokens[3].kind);
    TEST_ASSERT_EQUAL_HEX(reg_r2_id, tokens[3].value.operand.value.reg);
}

void test_lexer_immediate_and_symbol_tokens(void)
{

    char* input[] = {"JMP", "0x12345678"};
    int   count   = 2;

    Token* tokens = lexer(&test_suite_arena, input, count);

    TEST_ASSERT_EQUAL_INT(TOK_OPCODE, tokens[0].kind);

    TEST_ASSERT_EQUAL_INT(TOK_LITERAL, tokens[1].kind);
    TEST_ASSERT_EQUAL_HEX32(0x12345678, tokens[1].value.operand.value.literal.value.longValue);
}

void run_all_lexer_tests(void)
{
    RUN_TEST(test_lexer_opcode_and_register_tokens);
    RUN_TEST(test_lexer_immediate_and_symbol_tokens);
}
