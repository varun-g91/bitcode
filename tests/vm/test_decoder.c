
#include "logger.h"
#include "test_common.h"
#include "unity.h"
#include "unity_internals.h"
#include "vm.h"
#include <stdint.h>

// --- Forward Declarations for Test Runner ---
void test_decoder_io_instructions(void);
void test_decoder_mov_immediate(void);
void test_decoder_mov_reg_indirect(void);
void test_decoder_load_addr(void);
void test_decoder_arithmetic_base_offset(void);
void test_decoder_control_flow_jmp(void);
void test_decoder_stack_push(void);

// =================================================================
// 1. I/O Instructions: PRINT_CHR R0
// =================================================================
void test_decoder_io_instructions(void)
{
    const uint8_t      instruction_print_chr[INSTRUCTION_SIZE] = {OP_PRINT_CHR, REG_R0, 0x00, 0x00,
                                                                  0x00,         0x00,   0x00, 0x00};
    DecodedInstruction decoded_instruction;
    int8_t result = decode_instruction(vm_ctx, instruction_print_chr, &decoded_instruction);

    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(OP_PRINT_CHR, decoded_instruction.opcode);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_REG_DIRECT, decoded_instruction.operands[0].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R0, decoded_instruction.operands[0].value.reg_id);
}

// =================================================================
// 2. Data Transfer: MOV R1, 100 (Reg Direct, Imm Int)
// =================================================================
void test_decoder_mov_immediate(void)
{
    const uint8_t      instruction_mov[INSTRUCTION_SIZE] = {OP_MOV, REG_R1, 0x00, 100,
                                                            0x00,   0x00,   0x00, 0x02};
    DecodedInstruction decoded_instruction;
    int8_t             result = decode_instruction(vm_ctx, instruction_mov, &decoded_instruction);

    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(OP_MOV, decoded_instruction.opcode);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_REG_DIRECT, decoded_instruction.operands[0].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R1, decoded_instruction.operands[0].value.reg_id);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_IMM_INT, decoded_instruction.operands[1].mode);
    TEST_ASSERT_EQUAL_UINT32(100, decoded_instruction.operands[1].value.address_or_value);
}

// =================================================================
// 3. Data Transfer: MOV R3, [R5] (Reg Direct, Reg Indirect)
// =================================================================
void test_decoder_mov_reg_indirect(void)
{
    const uint8_t instruction_mov_r3_indir_r5[INSTRUCTION_SIZE] = {OP_MOV, REG_R3, REG_R5, 0x00,
                                                                   0x00,   0x00,   0x00,   0x08};
    DecodedInstruction decoded_instruction;
    int8_t result = decode_instruction(vm_ctx, instruction_mov_r3_indir_r5, &decoded_instruction);

    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(OP_MOV, decoded_instruction.opcode);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_REG_DIRECT, decoded_instruction.operands[0].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R3, decoded_instruction.operands[0].value.reg_id);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_REG_INDIRECT, decoded_instruction.operands[1].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R5, decoded_instruction.operands[1].value.reg_id);
}

// =================================================================
// 4. Address Loading: LOAD_ADDR R1, 0xDEADBEEF (Reg Direct, Imm Address)
// =================================================================
void test_decoder_load_addr(void)
{
    const uint8_t      instruction_load_addr[INSTRUCTION_SIZE] = {OP_LOAD_ADDR, REG_R1, 0x00, 0xEF,
                                                                  0xBE,         0xAD,   0xDE, 0x04};
    DecodedInstruction decoded_instruction;
    int8_t result = decode_instruction(vm_ctx, instruction_load_addr, &decoded_instruction);

    LOG_DEBUG("Immediate address in decoded instruction operand: %ld\n",
              decoded_instruction.operands[1].value.address_or_value);

    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(OP_LOAD_ADDR, decoded_instruction.opcode);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_REG_DIRECT, decoded_instruction.operands[0].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R1, decoded_instruction.operands[0].value.reg_id);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_IMM_ADDR, decoded_instruction.operands[1].mode);
    TEST_ASSERT_EQUAL_UINT32(0xDEADBEEF, decoded_instruction.operands[1].value.address_or_value);
}

// =================================================================
// 5. Arithmetic: ADD R0, [R7 + 0x1A] (Reg Direct, Base Offset)
// =================================================================
void test_decoder_arithmetic_base_offset(void)
{
    const uint8_t instruction_add_base_offset[INSTRUCTION_SIZE] = {OP_ADD, REG_R0, REG_R7, 0x1A,
                                                                   0x00,   0x00,   0x00,   0x0C};
    DecodedInstruction decoded_instruction;
    int8_t result = decode_instruction(vm_ctx, instruction_add_base_offset, &decoded_instruction);

    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(OP_ADD, decoded_instruction.opcode);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_REG_DIRECT, decoded_instruction.operands[0].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R0, decoded_instruction.operands[0].value.reg_id);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_BASE_OFFSET, decoded_instruction.operands[1].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R7, decoded_instruction.operands[1].value.base_and_offset.reg_id);
    TEST_ASSERT_EQUAL_UINT32(0x1A, decoded_instruction.operands[1].value.base_and_offset.offset);
}

// =================================================================
// 6. Control Flow: JMP 0x100 (Immediate Address)
// =================================================================
void test_decoder_control_flow_jmp(void)
{
    const uint8_t      instruction_jmp_abs[INSTRUCTION_SIZE] = {OP_JMP, 0x00, 0x00, 0x00,
                                                                0x01,   0x00, 0x00, 0x20};
    DecodedInstruction decoded_instruction;
    int8_t result = decode_instruction(vm_ctx, instruction_jmp_abs, &decoded_instruction);

    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(OP_JMP, decoded_instruction.opcode);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_IMM_ADDR, decoded_instruction.operands[0].mode);
    TEST_ASSERT_EQUAL_UINT32(0x100, decoded_instruction.operands[0].value.address_or_value);
}

// =================================================================
// 7. Stack: PUSH R4 (Reg Direct)
// =================================================================
void test_decoder_stack_push(void)
{
    const uint8_t      instruction_push_r4[INSTRUCTION_SIZE] = {OP_PUSH, REG_R4, 0x00, 0x00,
                                                                0x00,    0x00,   0x00, 0x00};
    DecodedInstruction decoded_instruction;
    int8_t result = decode_instruction(vm_ctx, instruction_push_r4, &decoded_instruction);

    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(OP_PUSH, decoded_instruction.opcode);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_REG_DIRECT, decoded_instruction.operands[0].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R4, decoded_instruction.operands[0].value.reg_id);
}

// =================================================================
// Test Runner Function
// =================================================================
void run_all_decoder_tests()
{
    RUN_TEST(test_decoder_io_instructions);
    RUN_TEST(test_decoder_mov_immediate);
    RUN_TEST(test_decoder_mov_reg_indirect);
    RUN_TEST(test_decoder_load_addr);
    RUN_TEST(test_decoder_arithmetic_base_offset);
    RUN_TEST(test_decoder_control_flow_jmp);
    RUN_TEST(test_decoder_stack_push);
}
