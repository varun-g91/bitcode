#include "token_stream.h"
#include "arena_allocator.h"
#include "lexer.h"
#include "logger.h"
#include <stdlib.h>

Token peek(TokenStream* ts) {
    if (ts->current == NULL) {
        Token tmp = {0};
        LOG_ERROR("Current token is null\n");
        return tmp;
    }
    Token token = ts->current->token;
    return token;
}

Token consume(TokenStream* ts) {
    if (ts->current == NULL) {
        LOG_ERROR("Current token null\n");
        Token tmp = {0};
        return tmp;
    }

    Token consumed_token = ts->current->token;
    ts->current          = ts->current->next;
    return consumed_token;
}

bool is_matching(TokenStream* ts, Opcode expected_opcode) {
    Opcode current_op = ts->current->token.value.opcode;
    if (current_op != expected_opcode) {
        return false;
    }
    return true;
}

TokenStream* build_token_stream(MemoryArena* arena, Token* all_tokens, int total_token_count) {
    if (all_tokens == NULL || total_token_count <= 0) {
        return NULL;
    }

    // Allocate the main stream structure in the arena
    TokenStream* stream = (TokenStream*)arena_alloc(arena, sizeof(TokenStream));
    if (stream == NULL) {
        LOG_ERROR("Failed to allocate memory for token stream\n");
        return NULL;
    }

    TokenNode* head_node    = NULL;
    TokenNode* current_node = NULL;

    // Start loop from the beginning of the tokens array
    for (int i = 0; i < total_token_count; i++) {
        TokenNode* new_node = (TokenNode*)arena_alloc(arena, sizeof(TokenNode));
        if (new_node == NULL) {
            LOG_ERROR("Failed to allocate memory for token stream node\n");
            // NOTE: No cleanup needed here, relies on final arena_free
            return NULL;
        }

        // Copy the token data into the new node
        new_node->token = all_tokens[i];
        new_node->next  = NULL;

        if (i == 0) {
            // First node
            head_node    = new_node;
            current_node = new_node;
        } else {
            // Subsequent nodes
            current_node->next = new_node;
            current_node       = new_node;
        }
    }

    // Finalize the stream structure
    stream->head    = head_node;
    stream->current = head_node;

    return stream;
}
