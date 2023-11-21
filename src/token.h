#ifndef TOKEN_HEADER
#define TOKEN_HEADER

#include <stdlib.h>

typedef enum TokenType {
    ON,
    OFF,
    AND,
    OR,
    GRP_OPEN,
    GRP_CLOSE,
} TokenType;

typedef struct Token {
    TokenType type;
    char * name;
} Token;


void print_tokens(Token * tokens, size_t tokens_len);
void print_token(Token * token);
void show_token(char * destination, Token * token);

#endif
