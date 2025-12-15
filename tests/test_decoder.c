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
// 1. I/O Instructions: PRINT_CHR R0 (Assumed)
// =================================================================
void test_decoder_io_instructions(void)
{
    // Assumed: PRINT_CHR R0. Opcode 0x00, R0 ID 0x00. Metadata 0x00 (AM1=RegDirect, AM2=None)
    const uint8_t      instruction_print_chr[INSTRUCTION_SIZE] = {0x00, 0x00, 0x00, 0x00,
                                                                  0x00, 0x00, 0x00, 0x00};
    DecodedInstruction decoded_instruction;
    int8_t             result;

    result = decode_instruction(vm_ctx, instruction_print_chr, &decoded_instruction);

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
    // MOV R1, 100. Opcode 0x02, R1 ID 0x01, Imm Value 100 (0x64). Metadata 0b00000010 = 0x02
    const uint8_t      instruction_mov[INSTRUCTION_SIZE] = {0x02, 0x01, 0x00, 0x64,
                                                            0x00, 0x00, 0x00, 0x02};
    DecodedInstruction decoded_instruction;
    int8_t             result;

    result = decode_instruction(vm_ctx, instruction_mov, &decoded_instruction);

    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(OP_MOV, decoded_instruction.opcode);
    TEST_ASSERT_EQUAL_INT8(VM_AM_REG_DIRECT, decoded_instruction.operands[0].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R1, decoded_instruction.operands[0].value.reg_id);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_IMM_INT, decoded_instruction.operands[1].mode);
    TEST_ASSERT_EQUAL_UINT32(100, decoded_instruction.operands[1].value.address_or_value);
}

// =================================================================
// 3. Data Transfer: MOV R3, [R5] (Reg Direct, Reg Indirect)
// =================================================================
void test_decoder_mov_reg_indirect(void)
{
    // MOV R3, [R5]. Opcode 0x02, R3 ID 0x03, R5 ID 0x05. Metadata 0b00000000 = 0x20
    const uint8_t      instruction_mov_r3_indir_r5[INSTRUCTION_SIZE] = {0x02, 0x03, 0x05, 0x00,
                                                                        0x00, 0x00, 0x00, 0x08};
    DecodedInstruction decoded_instruction;
    int8_t             result;

    result = decode_instruction(vm_ctx, instruction_mov_r3_indir_r5, &decoded_instruction);

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
    // LOAD_ADDR R1, 0xDEADBEEF. Opcode 0x03, R1 ID 0x01, Addr 0xDEADBEEF. Metadata 0b000100 = 0x04
    const uint8_t      instruction_load_addr[INSTRUCTION_SIZE] = {0x03, 0x01, 0x00, 0xEF,
                                                                  0xBE, 0xAD, 0xDE, 0x04};
    DecodedInstruction decoded_instruction;
    int8_t             result;

    result = decode_instruction(vm_ctx, instruction_load_addr, &decoded_instruction);
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
    // ADD R0, [R7 + 0x1A]. Opcode 0x04, R0 ID 0x00, R7 ID 0x07, Offset 0x1A. Metadata 0b00001100 =
    // 0x0C
    const uint8_t      instruction_add_base_offset[INSTRUCTION_SIZE] = {0x04, 0x00, 0x07, 0x1A,
                                                                        0x00, 0x00, 0x00, 0x0C};
    DecodedInstruction decoded_instruction;
    int8_t             result;

    result = decode_instruction(vm_ctx, instruction_add_base_offset, &decoded_instruction);

    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, result);
    TEST_ASSERT_EQUAL_UINT8(OP_ADD, decoded_instruction.opcode);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_REG_DIRECT, decoded_instruction.operands[0].mode);
    TEST_ASSERT_EQUAL_UINT8(REG_R0, decoded_instruction.operands[0].value.reg_id);
    TEST_ASSERT_EQUAL_UINT8(VM_AM_BASE_OFFSET, decoded_instruction.operands[1].mode);
    TEST_ASSERT_EQUAL_UINT8(
        REG_R7, decoded_instruction.operands[1].value.base_and_offset.reg_id); // Base Register
    TEST_ASSERT_EQUAL_UINT32(
        0x1A,
        decoded_instruction.operands[1].value.base_and_offset.offset); // Offset
}

// =================================================================
// 6. Control Flow: JMP 0x100 (Immediate Address)
// =================================================================
void test_decoder_control_flow_jmp(void)
{
    // JMP 0x100. Opcode 0x15, Target 0x100. Metadata 0b000100 = 0x10 (AM2=ImmAddr, AM1=None)
    const uint8_t      instruction_jmp_abs[INSTRUCTION_SIZE] = {0x15, 0x00, 0x00, 0x00,
                                                                0x01, 0x00, 0x00, 0x20};
    DecodedInstruction decoded_instruction;
    int8_t             result;

    result = decode_instruction(vm_ctx, instruction_jmp_abs, &decoded_instruction);

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
    // PUSH R4. Opcode 0x18, R4 ID 0x04. Metadata 0b000000 = 0x00 (AM1=RegDirect, AM2=None)
    const uint8_t      instruction_push_r4[INSTRUCTION_SIZE] = {0x18, 0x04, 0x00, 0x00,
                                                                0x00, 0x00, 0x00, 0x00};
    DecodedInstruction decoded_instruction;
    int8_t             result;

    result = decode_instruction(vm_ctx, instruction_push_r4, &decoded_instruction);

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
