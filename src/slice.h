#ifndef SLICE_HEADER
#define SLICE_HEADER

#include "token.h"

typedef struct Slice {
    Token * arr;
    int start;
    int end;
} Slice;

Slice new_slice(Token * arr, int start, int end);

Token token_at(Slice slice, int index);
Token last_token(Slice slice);
#endif
