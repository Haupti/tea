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
    NOT,
} TokenType;

typedef struct Token {
    TokenType type;
} Token;


void print_tokens(Token * tokens, size_t tokens_len);
void print_token(Token * token);
void show_token(char * destination, Token * token);

int is_value(Token token);
int is_combinator(Token token);
int is_modifier(Token token);
int is_grp_open(Token token);
#endif
