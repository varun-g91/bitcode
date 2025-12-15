#ifndef VM_UTILS_H
#define VM_UTILS_H

#include "vm.h"
#include <stdint.h>

uint32_t vm_allocate_string(VMContext*, const char*);
void     populate_operand(VMOperand*, uint8_t, uint32_t);
void     vm_print_string(VMContext*, uint32_t);

#endif
