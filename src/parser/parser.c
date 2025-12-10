// #include "parser.h"
// #include "lexer.h"
// #include "logger.h"
//
// int calculate_instruction_size(Token* tokens, int count) {
//     int instruction_size = 0;
//     if (!tokens) {
//         LOG_ERROR("Tokens array was found to be null\n");
//         return -1;
//     }
//     for (int i = 0; i < count; i++) {
//         if (tokens[i].kind == TOK_SEPARATOR && tokens[i].value.sep == SEP_EOL) {
//             break;
//         }
//
//         if (tokens[i].kind == TOK_OPCODE || tokens[i].kind == TOK_REGISTER) {
//             instruction_size += 1;
//         }
//
//         if (tokens[i].kind == TOK_LITERAL) {
//             switch (tokens[i].value.operand.value.literal.type) {
//             case LIT_LONG_LONG:
//                 instruction_size +=
//                 sizeof(tokens[i].value.operand.value.literal.value.longValue);
//             case LIT_CHAR:
//                 instruction_size +=
//                 sizeof(tokens[i].value.operand.value.literal.value.charValue);
//             case LIT_STR:
//                 instruction_size +=
//                 sizeof(tokens[i].value.operand.value.literal.value.stringValue);
//             }
//         }
//     }
//     return instruction_size;
// }
//
// // void first_pass(TokenStream* ts, AssemblerContext* asm_ctx) {}
//
// // void run_parser(MemoryArena* arena, TokenStream* ts, AssemblerContext* asm_ctx,
// //                 SymbolTable* symbol_table) {
// //
// //     while (ts->current != NULL) {
// //         Token token = ts->current->token;
// //         if (token.kind == TOK_SEPARATOR && token.value.sep == SEP_EOL) {
// //             asm_ctx->location_counter += 8; // location_counter is a uint32_t type
// //         }
// //     }
// // }
// //
// // void parse_program() {}
// //
// // void parse_line(AssemblerContext* asm_ctx, MemoryArena* arena, TokenStream* stream,
// //                 int pass_number) {
// //
// //     uint32_t line_size = 0;
// //     uint32_t address   = asm_ctx->location_counter;
// //     if (peek(stream).kind == TOK_LABEL_DEF) {
// //         size_t len = strlen(peek(stream).value.label_def) - 2;
// //         char*  symbol;
// //         strncpy(symbol, peek(stream).value.label_def, len);
// //         symbol[len] = '\0';
// //
// //         symbol_table_add(arena, asm_ctx->symbol_table, symbol, address);
// //     }
// // }
// //
// // void parse_label_def() {}
// //
// // void parse_instruction() {}
// //
// // void parse_opcode() {}
// //
// // void parse_comment() {}
// //
// // void consume_new_line() {}
// //
// // void parse_operand() {}
// //
// // void parse_integer() {}
// //
// // void parse_address() {}
// //
// // void parse_string() {}
// //
// // void parse_register(TokenStream* ts, Instruction* instruction) {
// //     if (ts == NULL || ts->current == NULL) {
// //         LOG_ERROR("Current stream or token is null\n");
// //         return;
// //     }
// //     Token current_token = peek(ts);
// //     if (current_token.kind == TOK_REGISTER) {
// //         LOG_DEBUG("Found register, adding onto instruction list.\n");
// //         consume(ts);
// //     }
// // }
