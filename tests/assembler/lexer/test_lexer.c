#include "lexer.h"
#include "test_common.h"
#include "unity.h"
#include <stdint.h>

#define MAX_ARENA_SIZE (1 * 1024 * 1024) // 1 MB

const uint8_t mov_id    = 0x02;
const uint8_t reg_r1_id = 0x01;
const uint8_t reg_r2_id = 0x02;

void run_all_lexer_tests(void);

void test_lexer_opcode_and_register_tokens(void)
{
    // Test case: MOV R1, R2
    char* input[] = {"mov", "r1", ",", "r2"};
    int   count   = 4;

    Token* tokens = lexer(&lexer_arena, input, count);

    TEST_ASSERT_EQUAL_INT(TOK_IDENTIFIER, tokens[0].kind);
    TEST_ASSERT_EQUAL_STRING("mov", tokens[0].value.identifier);

    TEST_ASSERT_EQUAL_INT(TOK_REGISTER, tokens[1].kind);
    TEST_ASSERT_EQUAL_HEX(reg_r1_id, tokens[1].value.reg);

    TEST_ASSERT_EQUAL_INT(TOK_SEPARATOR, tokens[2].kind);

    TEST_ASSERT_EQUAL_INT(TOK_REGISTER, tokens[3].kind);
    TEST_ASSERT_EQUAL_HEX(reg_r2_id, tokens[3].value.reg);
}

void test_lexer_immediate_and_symbol_tokens(void)
{

    char* input[] = {"jmp", "0x12345678"};
    int   count   = 2;

    Token* tokens = lexer(&lexer_arena, input, count);

    TEST_ASSERT_EQUAL_INT(TOK_IDENTIFIER, tokens[0].kind);

    TEST_ASSERT_EQUAL_INT(TOK_LITERAL, tokens[1].kind);
    TEST_ASSERT_EQUAL_HEX32(0x12345678, tokens[1].value.literal.value.longValue);
}

void run_all_lexer_tests(void)
{
    RUN_TEST(test_lexer_opcode_and_register_tokens);
    RUN_TEST(test_lexer_immediate_and_symbol_tokens);
}
