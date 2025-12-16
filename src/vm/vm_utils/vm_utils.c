#include "vm_utils.h"
#include "logger.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint32_t vm_allocate_string(VMContext* ctx, const char* host_string)
{
    size_t len           = strlen(host_string);
    size_t required_size = len + 1;

    if (ctx->hp + required_size >= (HEAP_START + HEAP_SIZE) - 1)
    {
        LOG_ERROR("Allocation failed. Not enough space in heap\n");
        return 0;
    }

    uint32_t vm_mem_address = ctx->hp + required_size;
    memcpy(&ctx->memory[vm_mem_address], host_string, required_size);

    ctx->hp += required_size;

    return vm_mem_address;
}

void populate_operand(VMOperand* operand, uint8_t raw_id_byte, uint32_t imm_addr_or_val)
{
    switch (operand->mode)
    {
    case VM_AM_REG_DIRECT:
    case VM_AM_REG_INDIRECT:
        operand->value.reg_id = raw_id_byte;
        break;
    case VM_AM_BASE_OFFSET:
    {
        operand->value.base_and_offset.offset = imm_addr_or_val;
        operand->value.base_and_offset.reg_id = raw_id_byte;
        break;
    }

    case VM_AM_IMM_INT:
    case VM_AM_IMM_ADDR:
    case VM_AM_PC_RELATIVE:
        operand->value.address_or_value = imm_addr_or_val;
        break;
    case VM_AM_NONE:
        break;
    default:
        LOG_ERROR("Unknown addrssing mode\n");
        break;
    }
}

void vm_print_string(VMContext* ctx, uint32_t address)
{
    uint32_t curr_addr = address;
    char*    string    = (char*) ctx->memory + address;
    char*    i         = string;
    while (*i != '\0' && curr_addr < MEM_SIZE)
    {
        fprintf(stdout, "%c", *i);
        i++;
        curr_addr++;
    }
    fprintf(stdout, "\n");
    if (curr_addr >= MEM_SIZE)
    {
        LOG_WARN("Exited due to pointer crossing memory boundry\n");
    }
}

int8_t read_header_u16(FILE* f, uint16_t* out)
{
    uint8_t b[2];
    if (fread(b, 1, 2, f) != 2)
    {
        return VM_ERR_INVALID_BYTECODE;
    }
    *out = (uint16_t) b[0] | ((uint16_t) b[1] << 8);
    return VM_EXIT_SUCCESS;
}

int8_t read_header_u32(FILE* f, uint32_t* out)
{
    uint8_t b[4];
    if (fread(b, 1, 4, f) != 4)
    {
        return VM_ERR_INVALID_BYTECODE;
    }
    *out = (uint32_t) b[0] | ((uint32_t) b[1] << 8) | ((uint32_t) b[2] << 16) |
           ((uint32_t) b[3] << 24);

    return VM_EXIT_SUCCESS;
}

int8_t parse_header(BytecodeFileHeader* out, FILE* f)
{
    int8_t status;

    status = read_header_u32(f, &out->magic_number);
    if (status == VM_ERR_INVALID_BYTECODE)
    {
        return status;
    }

    status = read_header_u16(f, &out->version_number);
    if (status == VM_ERR_INVALID_BYTECODE)
    {
        return status;
    }

    status = read_header_u32(f, &out->code_len);
    if (status == VM_ERR_INVALID_BYTECODE)
    {
        return status;
    }

    status = read_header_u32(f, &out->entry_point);
    if (status == VM_ERR_INVALID_BYTECODE)
    {
        return status;
    }
    status = read_header_u32(f, &out->rodata_len);
    if (status == VM_ERR_INVALID_BYTECODE)
    {
        return status;
    }

    status = read_header_u32(f, &out->data_len);
    if (status == VM_ERR_INVALID_BYTECODE)
    {
        return status;
    }

    return VM_EXIT_SUCCESS;
}
