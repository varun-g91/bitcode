#ifndef TOKEN_STREAM_H
#define TOKEN_STREAM_H

#include "arena_allocator.h"
#include "lexer.h"

typedef struct _TokenNode
{
    Token              token;
    struct _TokenNode* next;
} TokenNode;

typedef struct
{
    TokenNode* head;
    TokenNode* current;
} TokenStream;

Token        peek(TokenStream*);
Token        consume(TokenStream*);
TokenStream* build_token_stream(MemoryArena*, Token*, int);

#endif
