// LOCAL LIBRARY
#include "vm.h"
#include "arena_allocator.h"
#include "instruction_format_table.h"
#include "lexer.h"
#include "logger.h"

// STANDARD LIBRARY
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define VM_OPERAND_1_INDEX 0
#define VM_OPERAND_2_INDEX 1
#define VM_IMM_OPERAND_INDEX 2

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
    ctx->memory = (uint8_t*) malloc(MEM_SIZE);
    if (ctx->memory == NULL)
    {
        LOG_ERROR("Error allocating VM memory\n");
        return NULL;
    }

    memset(ctx, 0, sizeof(VMContext));
    ctx->state = VM_STATE_HALTED;
    return ctx;
}

bool load_bytecode(VMContext* ctx, const uint8_t* bytecode, size_t size)
{
    if (size > MEM_SIZE)
    {
        LOG_ERROR("Failed to load bytecode. Size too big for our tiny VM\n");
        return false;
    }
    memcpy(&ctx->memory[CODE_START], bytecode, size);
    ctx->pc = CODE_START;
    ctx->sp = MEM_SIZE - 1;
    return true;
}

uint8_t* fetch_instruction(VMContext* ctx, MemoryArena* arena)
{
    if (ctx->pc > MEMORY_LIMIT)
    {
        LOG_ERROR("PC out of bounds\n");
        return NULL;
    }

    uint8_t* buffer = (uint8_t*) arena_alloc(arena, INSTRUCTION_SIZE * sizeof(uint8_t));
    if (buffer == NULL)
    {
        LOG_ERROR("Failed to allocate instruction buffer\n");
        return NULL;
    }
    memcpy(buffer, ctx->memory + ctx->pc, INSTRUCTION_SIZE);
    return buffer;
}

DecodedInstruction* decode_instruction(VMContext* ctx, MemoryArena* arena)
{

    const uint8_t* instruction = fetch_instruction(ctx, arena);

    if (instruction == NULL)
    {
        LOG_ERROR("Instruction is NULL\n");
        return NULL;
    }

    DecodedInstruction* out =
        (DecodedInstruction*) arena_calloc(arena, 0, sizeof(DecodedInstruction));

    uint8_t           opcode   = instruction[OPCODE_INDEX];
    uint8_t           metadata = instruction[METADATA_INDEX];
    const OpcodeInfo* info     = &opcode_info[opcode];
    out->opcode                = (Opcode) opcode;
    out->metadata_flags        = metadata;

    for (int i = 0; i < info->operand_count; i++)
    {
        const uint8_t reg_id_ptr = instruction[i + 1];
        switch (info->operands[i])
        {
        case OT_ANY_SOURCE:
        {
            if (metadata & META_OP2_IMM)
            { // immediate value
                int32_t        immediate_value;
                const uint8_t* immediate_ptr = instruction + IMMEDIATE_VALUE_START;
                memcpy(&immediate_value, immediate_ptr, sizeof(int32_t));
                out->operands[i].kind            = VM_OT_IMMEDIATE;
                out->operands[i].value.immediate = immediate_value;
                break;
            }
            else
            { // register
                uint8_t reg_id                = reg_id_ptr;
                out->operands[i].kind         = VM_OT_REGISTER;
                out->operands[i].value.reg_id = reg_id;
                break;
            }
        }
        case OT_REGISTER:
        {
            uint8_t reg_id                = reg_id_ptr;
            out->operands[i].kind         = VM_OT_REGISTER;
            out->operands[i].value.reg_id = reg_id;
            break;
        }
        case OT_IMMEDIATE:
        {
            int32_t        immediate_value;
            const uint8_t* immediate_ptr = instruction + IMMEDIATE_VALUE_START;
            memcpy(&immediate_value, immediate_ptr, sizeof(int32_t));
            out->operands[i].kind            = VM_OT_IMMEDIATE;
            out->operands[i].value.immediate = immediate_value;
            break;
        }
        case OT_SYMBOL:
        {
            uint32_t       address_value;
            const uint8_t* address_ptr = instruction + IMMEDIATE_VALUE_START;
            memcpy(&address_value, address_ptr, sizeof(uint32_t));
            out->operands[i].kind          = VM_OT_ADDRESS;
            out->operands[i].value.address = address_value;
            break;
        }
        case OT_NONE:
        {
            out->operands[i].kind = VM_OT_NONE;
            break;
        }
        }
    }
    return out;
}

void read_bytecode(VMContext* ctx)
{
    // Set the VM state as running
    ctx->state = VM_STATE_RUNNING;

    while (ctx->state == VM_STATE_RUNNING)
    {
    }
}

// void execute_bytecode(VMContext* ctx) {}
