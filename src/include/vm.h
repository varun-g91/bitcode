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

typedef enum
{
    VM_STATE_RUNNING,
    VM_STATE_HALTED,
    VM_STATE_ERROR,
} VMStates;

typedef struct
{
    VMStates     state;
    uint64_t     registers[16];
    uint8_t*     memory;
    const size_t memory_size;
    uint64_t     pc;
    uint64_t     sp;
    uint64_t     bp;
    uint64_t     flags[4];
} VMContext;

typedef enum
{
    VM_OT_REGISTER,
    VM_OT_IMMEDIATE,
    VM_OT_ADDRESS,
    VM_OT_NONE,
} VMOperandKind;

typedef union
{
    uint8_t  reg_id;
    int32_t  immediate;
    uint32_t address;
} VMOperandValue;

typedef struct
{
    VMOperandKind  kind;
    VMOperandValue value;
} VMOperand;

typedef struct
{
    Opcode    opcode;
    uint8_t   metadata_flags;
    VMOperand operands[3];
} DecodedInstruction;

VMContext* vm_create();
bool       load_bytecode(VMContext*, const uint8_t*, size_t size);
void       execute_bytecode(VMContext*);

/*
 * Opcode   Operand1 Operand2 |----------Immediate Value--------| Metadata
 * 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
 *
 */
