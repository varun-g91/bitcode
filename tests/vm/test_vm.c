#include "test_common.h"
#include "unity.h"
#include "unity_internals.h"
#include "vm.h"
#include <stdio.h>
void run_all_vm_tests(void);

void test_full_vm_cycle();
void test_vm_loader();

void test_full_vm_cycle()
{
    const char* filename = "/home/varun/Personal/Projects/C/bitlang/test.vmbc";
    int8_t      status   = run_vm(vm_ctx, filename);
    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, status);
}

void test_vm_loader()
{
    const char* filename = "/home/varun/Personal/Projects/C/bitlang/test.vmbc";
    int8_t      status   = load_bytecode(vm_ctx, filename);
    TEST_ASSERT_EQUAL_INT8(VM_EXIT_SUCCESS, status);
}

void run_all_vm_tests()
{
    RUN_TEST(test_full_vm_cycle);
    RUN_TEST(test_vm_loader);
}
