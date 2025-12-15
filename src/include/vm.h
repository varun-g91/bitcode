#ifndef VM_H
#define VM_H

#include "lexer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// TOTAL MEMORY = 5 MB
#define MEM_SIZE (5 * 1024 * 1024)

// ---- SEGMENTS ----

// CODE: 512 KB
#define CODE_START 0x000000
#define CODE_SIZE 0x080000

// RODATA: 256 KB
#define RODATA_START 0x080000
#define RODATA_SIZE 0x040000

// DATA: 256 KB
#define DATA_START 0x0C0000
#define DATA_SIZE 0x040000

// HEAP: 3.5 MB
#define HEAP_START 0x100000
#define HEAP_SIZE 0x3C0000

// STACK: 512 KB
#define STACK_START 0x4C0000
#define STACK_SIZE 0x080000

// UTILITY MACORS
#define OPCODE_SIZE 1
#define OPERAND_SIZE 1
#define MEMORY_LIMIT 5242880
#define INSTRUCTION_SIZE 8
#define META_OP2_IMM 0x01 // bit 0
#define OPCODE_INDEX 0
#define OPERAND_1_INDEX 1
#define OPERAND_2_INDEX 2
#define IMMEDIATE_VALUE_START 3
#define METADATA_INDEX 7

#define METADATA_MASK 0b111
// helper macros
#define GET_SRC_MODE(metadata_byte) ((VMAddressingMode) (metadata_byte >> 1) & METADATA_MASK)

#define GET_DEST_MODE(metadata_byte) ((VMAddressingMode) (metadata_byte >> 4) & METADATA_MASK)

#define GET_GLOBAL_FLAG(metadata_byte) ((metadata_byte >> 7) & 0b1)

// types
typedef enum
{
    VM_STATE_RUNNING,
    VM_STATE_HALTED,
    VM_STATE_FATAL_ERROR,
    VM_STATE_SOFT_ERROR
} VMState;

typedef enum
{
    // --- 0-99: FATAL ERRORS (Immediate system halt required) ---
    VM_EXIT_SUCCESS = 0,

    // VM Initialization/Setup Errors
    VM_ERR_BYTECODE_TOO_LARGE       = 1, // Exceeds CODE_SIZE boundary
    VM_ERR_MEMORY_ALLOCATION_FAILED = 2, // OS couldn't give us MEM_SIZE

    // Critical Runtime Errors
    VM_ERR_INVALID_BYTECODE = 3, // Corrupted/unrecognized instruction format
    VM_ERR_OPCODE_NOT_FOUND = 4, // Unknown Opcode encountered
    VM_ERR_NULL_POINTER     = 5, // Attempt to dereference 0x000000
    VM_ERR_PC_OUT_OF_BOUNDS = 6,
    VM_ERR_UNKNOWN          = 99, // Catch-all for unhandled exceptions

    // --- 100-199: NON-FATAL ERRORS (Recoverable, returns control) ---

    // Memory/Stack Errors
    VM_ERR_STACK_OVERFLOW       = 100, // Push exceeded STACK_START
    VM_ERR_STACK_UNDERFLOW      = 101, // Pop on an empty stack
    VM_ERR_HEAP_OUT_OF_BOUNDS   = 102, // Access outside HEAP segment
    VM_ERR_MEMORY_OUT_OF_BOUNDS = 103,

    // Instruction/Operand Errors
    VM_ERR_REGISTER_NOT_FOUND      = 110, // Operand specifies non-existent register (e.g., R_9)
    VM_ERR_DIVIDE_BY_ZERO          = 111, // Attempted division by zero
    VM_ERR_INVALID_ADDRESSING_MODE = 112,
    VM_ERR_ILLEGAL_OPERATION       = 113,

    // I/O Errors (if you add I/O)
    VM_ERR_IO_READ_FAILED = 120, // Failed to read from an open stream

} VMErrorState;
typedef struct
{
    VMErrorState error_state;
    uint32_t     instruction_address;
} VMError;

typedef struct
{
    VMState      state;
    uint32_t     registers[8];
    uint8_t*     memory;
    const size_t memory_size;
    uint32_t     pc;
    uint32_t     sp;
    uint32_t     bp;
    uint32_t     hp;
    uint32_t     flags[4];
} VMContext;

typedef enum
{
    VM_AM_REG_DIRECT   = 0b000, // R_x
    VM_AM_IMM_INT      = 0b001, // 42
    VM_AM_IMM_ADDR     = 0b010, // $0x1000
    VM_AM_REG_INDIRECT = 0b100, // [R_x]
    VM_AM_BASE_OFFSET  = 0b110, // [R_x + Offset]
    VM_AM_PC_RELATIVE  = 0b111, // LABEL (for control flow)
    VM_AM_NONE         = 0b1110 // RET (0 operand inst)
} VMAddressingMode;

typedef struct
{
    VMAddressingMode mode;
    union
    {
        uint8_t  reg_id;
        uint32_t address_or_value;
        struct
        {
            uint8_t  reg_id;
            uint32_t offset;
        } base_and_offset;
    } value;
} VMOperand;

typedef struct
{
    Opcode    opcode;
    uint8_t   metadata_flags;
    VMOperand operands[2];
} DecodedInstruction;

typedef enum
{
    VM_VT_INT,
    VM_VT_STRING_PTR,
    VM_VT_UNKNOWN
} VMValueType;

typedef struct
{
    VMValueType type;
    union
    {
        uint32_t int_value;
        char*    string_ptr;
    } value;
} ResolvedValue;

int8_t handle_print_chr(VMContext*, DecodedInstruction);
int8_t handle_print_str(VMContext*, DecodedInstruction);
int8_t handle_mov(VMContext*, DecodedInstruction);
int8_t handle_load_addr(VMContext*, DecodedInstruction);
// int8_t handle_mul(VMContext*, DecodedInstruction);
// int8_t handle_div(VMContext*, DecodedInstruction);
// int8_t handle_mod(VMContext*, DecodedInstruction);
// int8_t handle_and(VMContext*, DecodedInstruction);
// int8_t handle_or(VMContext*, DecodedInstruction);
// int8_t handle_not(VMContext*, DecodedInstruction);
// int8_t handle_unknown(VMContext*, DecodedInstruction);
// int8_t handle_cmp(VMContext*, DecodedInstruction);
// int8_t handle_jz(VMContext*, DecodedInstruction);
// int8_t handle_jnz(VMContext*, DecodedInstruction);
// int8_t handle_jeq(VMContext*, DecodedInstruction);
// int8_t handle_jgt(VMContext*, DecodedInstruction);
// int8_t handle_jge(VMContext*, DecodedInstruction);
// int8_t handle_jlt(VMContext*, DecodedInstruction);
// int8_t handle_jle(VMContext*, DecodedInstruction);
// int8_t handle_jmp(VMContext*, DecodedInstruction);
// int8_t handle_call(VMContext*, DecodedInstruction);
// int8_t handle_ret(VMContext*, DecodedInstruction);
// int8_t handle_push(VMContext*, DecodedInstruction);
// int8_t handle_pop(VMContext*, DecodedInstruction);
//
typedef int8_t (*InstructionHandler)(VMContext*, DecodedInstruction);

extern InstructionHandler opcode_handler[256];
// functions
VMContext* vm_create();
void       vm_destroy(VMContext*);
bool       load_bytecode(VMContext*, const uint8_t*, size_t);
int8_t     fetch_instruction(VMContext*, uint8_t*);
int8_t     decode_instruction(VMContext*, const uint8_t*, DecodedInstruction*);
int8_t     run_vm(VMContext*);
int8_t     execute_bytecode(VMContext*, DecodedInstruction*);
uint32_t   vm_allocate_string(VMContext*, const char*);
void       set_vm_error_state(VMContext*, VMError*, int8_t);
#endif // !VM_H0,  // MSB
