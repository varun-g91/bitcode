// LOCAL LIBRARY
#include "vm.h"
#include "instruction_format_table.h"
#include "lexer.h"
#include "logger.h"
#include "vm_utils.h"

// STANDARD LIBRARY
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

InstructionHandler opcode_handler[256] = {[OP_PRINT_CHR] = handle_print_chr,
                                          [OP_PRINT_STR] = handle_print_str,
                                          [OP_MOV]       = handle_mov,
                                          [OP_LOAD_ADDR] = handle_load_addr,
                                          // [OP_MUL]       = handle_mul,
                                          // [OP_DIV]       = handle_div,
                                          // [OP_MOD]       = handle_mod,
                                          // [OP_AND]       = handle_and,
                                          // [OP_OR]        = handle_or,
                                          // [OP_NOT]       = handle_not,
                                          // [OP_CMP]       = handle_cmp,
                                          // [OP_JZ]        = handle_jz,
                                          // [OP_JNZ]       = handle_jnz,
                                          // [OP_JEQ]       = handle_jeq,
                                          // [OP_JGT]       = handle_jgt,
                                          // [OP_JGE]       = handle_jge,
                                          // [OP_JLT]       = handle_jlt,
                                          // [OP_JLE]       = handle_jle,
                                          // [OP_JMP]       = handle_jmp,
                                          // [OP_CALL]      = handle_call,
                                          // [OP_RET]       = handle_ret,
                                          // [OP_PUSH]      = handle_push,
                                          // [OP_POP]       = handle_pop,
                                          // [OP_UNKNOWN]   = handle_unknown
                                          [OP_HALT] = handle_halt};

/*
 *   Creates initial vm state
 * */
VMContext* vm_create()
{
    VMContext* ctx = (VMContext*) malloc(sizeof(VMContext));
    if (ctx == NULL)
    {
        LOG_ERROR("Unable to allocate memory for VM State\n");
        return NULL;
    }
    memset(ctx, 0, sizeof(VMContext));
    ctx->memory = (uint8_t*) malloc(MEM_SIZE);
    if (ctx->memory == NULL)
    {
        LOG_ERROR("Error allocating VM memory\n");
        return NULL;
    }

    ctx->state = VM_STATE_HALTED;
    return ctx;
}

void vm_destroy(VMContext* ctx)
{
    printf("DEBUG: vm_destroy called.\n");
    if (ctx)
    {
        if (ctx->memory)
        {
            free(ctx->memory);
            printf("DEBUG: ctx->memory freed.\n");
        }
        free(ctx);
        printf("DEBUG: ctx freed.\n");
    }
    else
    {
        printf("DEBUG: vm_destroy called with NULL context.\n");
    }
}

void vm_terminate(VMContext* ctx, int8_t error_code, uint32_t pc)
{
    ctx->state = VM_STATE_FATAL_ERROR;
    LOG_ERROR("FATAL ERROR %d at PC: 0x%X\n", error_code, pc);
    vm_destroy(ctx);
    exit(error_code);
}

int8_t load_bytecode(VMContext* ctx, const char* file_name)
{
    BytecodeFileHeader header;

    FILE* bytecode_file = fopen(file_name, "rb");
    if (!bytecode_file)
    {
        LOG_ERROR("Failed to open file %s, check if the file exists and try again.\n");
        fclose(bytecode_file);
        return VM_ERR_IO_READ_FAILED;
    }

    int8_t status = parse_header(&header, bytecode_file);
    if (status != VM_EXIT_SUCCESS)
    {
        LOG_ERROR("Incorrect bytecode format\n");
        fclose(bytecode_file);
        return status;
    }

    if (header.code_len > CODE_SIZE)
    {
        LOG_ERROR("Failed to load bytecode. Size too big for our tiny VM\n");
        fclose(bytecode_file);
        return VM_ERR_INVALID_BYTECODE;
    }

    if (header.code_len == 0)
    {
        LOG_ERROR("Code segment cannot be empty.\n");
        fclose(bytecode_file);
        return VM_ERR_INVALID_BYTECODE;
    }

    if (header.version_number != BYTECODE_SUPPORTED_VERSION)
    {
        LOG_ERROR("Unsupported bytecode version.\n");
        fclose(bytecode_file);
        return VM_ERR_INVALID_BYTECODE;
    }

    if (header.data_len > DATA_SIZE)
    {
        LOG_ERROR("Data segment too large.\n");
        fclose(bytecode_file);
        return VM_ERR_INVALID_BYTECODE;
    }

    if (header.rodata_len > RODATA_SIZE)
    {
        LOG_ERROR("Read-Only Data segment too large.\n");
        fclose(bytecode_file);
        return VM_ERR_INVALID_BYTECODE;
    }

    size_t bytes_read = fread(&ctx->memory[CODE_START], 1, header.code_len, bytecode_file);
    if (bytes_read != header.code_len)
    {
        LOG_ERROR("Code segments seems to be truncated.\n");
        fclose(bytecode_file);
        return VM_ERR_INVALID_BYTECODE;
    }

    ctx->pc = CODE_START + header.entry_point;
    ctx->sp = STACK_START + STACK_SIZE;
    ctx->bp = STACK_START + STACK_SIZE;
    ctx->hp = HEAP_START;

    fclose(bytecode_file);

    return VM_EXIT_SUCCESS;
}

int8_t fetch_instruction(VMContext* ctx, uint8_t* out)
{
    if (ctx->pc > MEMORY_LIMIT)
    {
        LOG_ERROR("PC out of bounds while trying to fetch instruction\n");
        return VM_ERR_PC_OUT_OF_BOUNDS;
    }

    memcpy(out, ctx->memory + ctx->pc, INSTRUCTION_SIZE);
    ctx->pc += INSTRUCTION_SIZE;
    return VM_EXIT_SUCCESS;
}

int8_t decode_instruction(VMContext* ctx, const uint8_t* instruction, DecodedInstruction* out)
{
    const Opcode  opcode          = instruction[0];
    const uint8_t metadata        = instruction[METADATA_INDEX];
    uint32_t      imm_addr_or_val = *(uint32_t*) &instruction[IMMEDIATE_VALUE_START];

    out->opcode         = opcode;
    out->metadata_flags = metadata;
    const OpcodeInfo* info;
    if (opcode_info[opcode].name != NULL)
    {
        info = &opcode_info[opcode];
    }
    else
    {
        info = NULL;
    }

    if (info == NULL)
    {
        LOG_ERROR("Opcode %d does not exist\n", opcode);
        ctx->state = VM_STATE_FATAL_ERROR;
        return VM_ERR_OPCODE_NOT_FOUND;
    }

    if (info->operand_count >= 1)
    {
        VMAddressingMode dest_mode = (VMAddressingMode) GET_DEST_MODE(out->metadata_flags);
        out->operands[0].mode      = dest_mode;
        uint8_t raw_id_byte_0      = instruction[OPERAND_1_INDEX];
        populate_operand(&out->operands[0], raw_id_byte_0, imm_addr_or_val);
    }
    else
    {
        out->operands[0].mode = VM_AM_NONE;
    }

    if (info->operand_count == 2)
    {
        VMAddressingMode src_mode = GET_SRC_MODE(out->metadata_flags);
        printf("DEBUG: src_mode calculated: %u\n", src_mode);
        out->operands[1].mode = src_mode;
        uint8_t raw_id_byte_1 = instruction[OPERAND_2_INDEX];
        populate_operand(&out->operands[1], raw_id_byte_1, imm_addr_or_val);
    }
    else
    {
        out->operands[1].mode = VM_AM_NONE;
    }
    return VM_EXIT_SUCCESS;
}

int8_t run_vm(VMContext* ctx, const char* file_name)
{
    // VMError            error_state;
    DecodedInstruction decoded_instruction;

    uint8_t raw_instruction[INSTRUCTION_SIZE];
    int8_t  status;

    if (ctx->state != VM_STATE_HALTED)
    {
        LOG_WARN("VM is not in HALTED state. Starting anyway.");
    }

    ctx->state = VM_STATE_RUNNING;
    status     = load_bytecode(ctx, file_name);
    if (status != VM_EXIT_SUCCESS)
    {
        LOG_ERROR("Error %d while loading bytecode\n", status);
        vm_terminate(ctx, status, ctx->pc - INSTRUCTION_SIZE);
    }
    while (ctx->state == VM_STATE_RUNNING)
    {
        // Fetch
        status = fetch_instruction(ctx, raw_instruction);
        if (status != VM_EXIT_SUCCESS)
        {
            if (status == VM_ERR_PC_OUT_OF_BOUNDS)
            {
                ctx->state = VM_STATE_HALTED;
                break;
            }
            vm_terminate(ctx, status, ctx->pc - INSTRUCTION_SIZE);
        }

        // Decode
        status = decode_instruction(ctx, raw_instruction, &decoded_instruction);
        if (status != VM_EXIT_SUCCESS)
        {
            vm_terminate(ctx, status, ctx->pc - INSTRUCTION_SIZE);
        }

        // Execute
        status = execute_bytecode(ctx, &decoded_instruction);
        if (status != VM_EXIT_SUCCESS)
        {
            uint32_t instruction_pc = ctx->pc - INSTRUCTION_SIZE;

            if (status > VM_EXIT_SUCCESS && status <= VM_ERR_UNKNOWN)
            {
                vm_terminate(ctx, status, instruction_pc);
            }
            else if (status >= VM_ERR_STACK_OVERFLOW)
            {
                ctx->state = VM_STATE_SOFT_ERROR;
                LOG_ERROR("Soft error %d at PC: 0x%X\n", status, instruction_pc);
                return status;
            }
            else
            {
                LOG_ERROR("Unknown status code (%d) returned from execute routine", status);
                vm_terminate(ctx, VM_ERR_UNKNOWN, instruction_pc);
            }
        }
    }
    return VM_EXIT_SUCCESS;
}

int8_t execute_bytecode(VMContext* ctx, DecodedInstruction* instruction)
{
    VMState state = ctx->state;
    LOG_DEBUG("VM State: %d\n", state);
    Opcode opcode = instruction->opcode;
    LOG_DEBUG("Opcode: %x\n", opcode);
    int8_t status = opcode_handler[opcode](ctx, *instruction);
    if (status != VM_EXIT_SUCCESS)
    {
        LOG_ERROR("VM exited with error code %d\n", status);
        return status;
    }
    return VM_EXIT_SUCCESS;
}

int8_t handle_print_chr(VMContext* ctx, DecodedInstruction instruction)
{
    LOG_DEBUG("handle_print_chr called!\n");
    VMAddressingMode mode = instruction.operands[0].mode;

    if (mode == VM_AM_REG_DIRECT)
    {
        uint8_t reg_id = instruction.operands[0].value.reg_id;
        uint8_t chr    = (uint8_t) (ctx->registers[reg_id] & LSB_MASK);
        fprintf(stdout, "%c", (char) chr);
    }
    else
    {
        return VM_ERR_ILLEGAL_OPERATION;
    }

    return VM_EXIT_SUCCESS;
}

int8_t handle_print_str(VMContext* ctx, DecodedInstruction instruction)
{
    VMAddressingMode mode = instruction.operands[0].mode;

    if (mode == VM_AM_REG_INDIRECT)
    {
        uint8_t        reg_id  = instruction.operands[0].value.reg_id;
        const uint32_t address = ctx->registers[reg_id];
        if (address < MEM_SIZE)
        {
            vm_print_string(ctx, address);
        }
    }
    else if (mode == VM_AM_IMM_ADDR)
    {
        uint32_t address = instruction.operands[0].value.address_or_value;
        if (address < MEM_SIZE)
        {
            vm_print_string(ctx, address);
        }
    }
    else
    {
        return VM_ERR_ILLEGAL_OPERATION;
    }
    return VM_EXIT_SUCCESS;
}

int8_t handle_mov(VMContext* ctx, DecodedInstruction instruction)
{
    uint8_t  dest_register_id = instruction.operands[0].value.reg_id;
    uint32_t imm_addr_or_val  = instruction.operands[1].value.address_or_value;
    switch (instruction.operands[1].mode)
    {

    case VM_AM_REG_DIRECT:
    {
        uint8_t  src_reg_id              = instruction.operands[1].value.reg_id;
        uint32_t src_data                = ctx->registers[src_reg_id];
        ctx->registers[dest_register_id] = src_data;
        break;
    }
    case VM_AM_IMM_INT:
    {
        ctx->registers[dest_register_id] = imm_addr_or_val;
        break;
    }
    case VM_AM_IMM_ADDR:
    {
        uint32_t address = imm_addr_or_val;
        if (address > MEM_SIZE)
        {
            LOG_ERROR("Cannot access past the memory boundry\n");
            return VM_ERR_MEMORY_OUT_OF_BOUNDS;
        }
        else
        {
            uint8_t* src_data_ptr = ctx->memory + address;
            memcpy(&ctx->registers[dest_register_id], src_data_ptr, sizeof(uint32_t));
            break;
        }
    }
    case VM_AM_REG_INDIRECT:
    {
        uint8_t  src_reg_id = instruction.operands[1].value.reg_id;
        uint32_t address    = ctx->registers[src_reg_id];
        if (address > MEM_SIZE)
        {
            LOG_ERROR("Cannot access past the memory boundry\n");
            return VM_ERR_MEMORY_OUT_OF_BOUNDS;
        }
        else
        {
            uint8_t* src_data_ptr = ctx->memory + address;
            memcpy(&ctx->registers[dest_register_id], src_data_ptr, sizeof(uint32_t));
            break;
        }
    }

    case VM_AM_BASE_OFFSET:
    {
        uint8_t  src_reg_id   = instruction.operands[1].value.reg_id;
        uint32_t base_address = ctx->registers[src_reg_id];
        uint32_t offset       = imm_addr_or_val;
        if (base_address >= MEM_SIZE || base_address + offset >= MEM_SIZE)
        {
            LOG_ERROR("Cannot access past memory boundry\n");
            return VM_ERR_MEMORY_OUT_OF_BOUNDS;
        }
        else
        {
            uint8_t* src_data_ptr = ctx->memory + (base_address + offset);
            memcpy(&ctx->registers[dest_register_id], src_data_ptr, sizeof(uint32_t));
            break;
        }
    }

    case VM_AM_PC_RELATIVE:
        break;
    case VM_AM_NONE:
        break;
    }

    return VM_EXIT_SUCCESS;
}

int8_t handle_load_addr(VMContext* ctx, DecodedInstruction instruction)
{
    uint8_t  dest_register_id   = instruction.operands[0].value.reg_id;
    uint32_t imm_addr_or_offset = instruction.operands[1].value.address_or_value;
    switch (instruction.operands->mode)
    {

    case VM_AM_REG_DIRECT:
        break;

    case VM_AM_IMM_INT:
        break;
    case VM_AM_IMM_ADDR:
    {
        uint32_t address = imm_addr_or_offset;
        if (address >= MEM_SIZE)
        {
            LOG_ERROR("Cannot access past memory boundry\n");
            return VM_ERR_MEMORY_OUT_OF_BOUNDS;
        }
        else
        {
            ctx->registers[dest_register_id] = address;
            break;
        }
    }
    case VM_AM_REG_INDIRECT:
    {
        uint8_t  src_reg_id = instruction.operands[0].value.reg_id;
        uint32_t address    = ctx->registers[src_reg_id];
        if (address >= MEM_SIZE)
        {
            LOG_ERROR("Cannot access past memory boundry\n");
            return VM_ERR_MEMORY_OUT_OF_BOUNDS;
        }
        else
        {
            ctx->registers[dest_register_id] = address;
            break;
        }
    }
    case VM_AM_BASE_OFFSET:
    {
        uint8_t  src_reg_id   = instruction.operands[1].value.reg_id;
        uint32_t base_address = ctx->registers[src_reg_id];
        uint32_t offset       = imm_addr_or_offset;
        if (base_address >= MEM_SIZE || base_address + offset >= MEM_SIZE)
        {
            LOG_ERROR("Cannot access past memory boundry\n");
            return VM_ERR_MEMORY_OUT_OF_BOUNDS;
        }
        else
        {
            ctx->registers[src_reg_id] = (base_address + offset);
            break;
        }
    }
    case VM_AM_PC_RELATIVE:
    {
        uint32_t offset                  = imm_addr_or_offset;
        ctx->registers[dest_register_id] = ctx->pc + offset;
        break;
    }
    case VM_AM_NONE:
        break;
    }

    return VM_EXIT_SUCCESS;
}

int8_t handle_halt(VMContext* ctx, DecodedInstruction instruction)
{
    if (instruction.opcode == OP_HALT && ctx->state != VM_STATE_HALTED)
    {
        ctx->state = VM_STATE_HALTED;
        return VM_EXIT_SUCCESS;
    }
    return VM_EXIT_SUCCESS;
}
