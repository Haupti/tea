#ifndef CURSOR_HEADER
#define CURSOR_HEADER

#include <stdlib.h>
#include "token.h"

typedef struct Cursor {
    Token * arr;
    size_t start;
    size_t end;
    int position;
} Cursor;

Token next(Cursor * it);
int size(Cursor * it);
Token prev(Cursor * it);
// does not move the cursor, returns the current element
Token current(Cursor * it);
Cursor new_cursor(Token * arr, size_t start, size_t end);
Cursor copy_cursor(Cursor * original);
int has_next(Cursor * it);
int has_prev(Cursor * it);
void jump_to(Cursor * it, int pos);
// moves cursor position to start and returs the first element
Token start(Cursor * it);
// moves cursor position to end and returs the last element
Token end(Cursor * it);
void print_cursor(Cursor * it);
void print_cursor_content(Cursor * it);

#endif
