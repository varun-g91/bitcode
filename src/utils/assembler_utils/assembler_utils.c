#include "opcodes.h"
#include <stdbool.h>
#include <stddef.h>
size_t calculate_vm_data_size(const char* content)
{
    size_t bytes = 0;
    for (int i = 0; content[i] != '\0'; i++)
    {
        if (content[i] == '\\' && content[i + 1] != '\0')
        {
            bytes++;
            i++;
        }
        else
        {
            bytes++;
        }
    }
    size_t total = bytes + 1;

    return (total + 3) & ~3;
}

bool is_opcode(const char* identifier)
{
    if (opcode_lookup(identifier) == 0xFF)
    {
        return false;
    }
    else
    {
        return true;
    }
}
