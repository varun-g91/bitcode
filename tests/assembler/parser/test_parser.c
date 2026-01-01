#include "parser.h"
#include "test_common.h"
#include "token_stream.h"
#include "unity.h"
#include "unity_internals.h"

void test_parser(void);
void run_all_parser_tests(void);

void test_parser()
{
    TokenStream* test_parser_stream;
    Program      test_program = {.capcity = 100, .count = 4, .lines = 0};
    const char*  src          = "start:\n"
                                "mov r1, r2\n"
                                "add r1, 100\n"
                                "cmp\n"
                                "jz start\n";
    test_parser_stream        = lex_from_string(&test_parser_arena, src);
    int8_t status             = run_parser(&test_parser_arena, test_parser_stream, &test_program);

    TEST_ASSERT_NOT_NULL(test_program.lines);
    TEST_ASSERT_EQUAL_UINT32(5, test_program.count);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(100, test_program.capcity);

    TEST_ASSERT_EQUAL_INT8(0, status);
    TEST_ASSERT_EQUAL_STRING("start", test_program.lines[0].value.label);
    TEST_ASSERT_EQUAL_INT8(OP_MOV, test_program.lines[1].value.instruction.opcode);
    TEST_ASSERT_EQUAL_INT8(OP_ADD, test_program.lines[2].value.instruction.opcode);
    TEST_ASSERT_EQUAL_INT8(OP_CMP, test_program.lines[3].value.instruction.opcode);
    TEST_ASSERT_EQUAL_INT8(OP_JZ, test_program.lines[4].value.instruction.opcode);
    // mov r1, r2
    TEST_ASSERT_EQUAL_INT8(OT_REGISTER, test_program.lines[1].value.instruction.operands[0].type);
    TEST_ASSERT_EQUAL_INT8(OT_REGISTER, test_program.lines[1].value.instruction.operands[1].type);

    // add r1, 100
    TEST_ASSERT_EQUAL_INT8(OT_REGISTER, test_program.lines[2].value.instruction.operands[0].type);
    TEST_ASSERT_EQUAL_INT8(OT_IMMEDIATE_INT,
                           test_program.lines[2].value.instruction.operands[1].type);

    // jz start
    TEST_ASSERT_EQUAL_INT8(OT_SYMBOL, test_program.lines[4].value.instruction.operands[0].type);

    TEST_ASSERT_EQUAL_UINT32(
        100, test_program.lines[2].value.instruction.operands[1].value.literal.value.longValue);

    TEST_ASSERT_EQUAL_INT8(LINE_INSTRUCTION, test_program.lines[1].type);
    TEST_ASSERT_EQUAL_INT8(LINE_LABEL_DEF, test_program.lines[0].type);
    TEST_ASSERT_EQUAL_INT8(LINE_INSTRUCTION, test_program.lines[1].type);
    TEST_ASSERT_EQUAL_INT8(LINE_INSTRUCTION, test_program.lines[2].type);
    TEST_ASSERT_EQUAL_INT8(LINE_INSTRUCTION, test_program.lines[3].type);
    TEST_ASSERT_EQUAL_INT8(LINE_INSTRUCTION, test_program.lines[4].type);
}

void run_all_parser_tests() { RUN_TEST(test_parser); }
