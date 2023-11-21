#ifndef LEXER_HEADER
#define LEXER_HEADER

#include <stdlib.h>

typedef enum TokenType {
    ON,
    OFF,
    AND,
    OR,
    GRP_OPEN,
    GRP_CLOSE,
} TokenType;

// reads tokens and saves them to storage
// returns number of tokens read
// throws on failure while reading tokens
int read_tokens(TokenType * tokens, char * input, size_t input_len);

void print_token(TokenType * token);
void show_token(char * destination, TokenType * token);

#endif
