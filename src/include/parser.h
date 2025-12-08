#include "assembler_context.h"
#include "lexer.h"
#include "token_stream.h"
// Const memory
#define MEMORY_SIZE 1024
#define MAX_TOKENS_PER_INSTRUCTION 5
// Logging
int memory[MEMORY_SIZE];
int stack[MEMORY_SIZE];

void run_parser(MemoryArena*, TokenStream*, AssemblerContext*, SymbolTable*);
void parse_program();

void parse_line(AssemblerContext*, MemoryArena*, TokenStream*, int);

void parse_label_def();

void parse_instruction();

void parse_opcode();

void parse_comment();

void consume_new_line();

void parse_operand();

void parse_integer();

void parse_address();

void parse_string();
