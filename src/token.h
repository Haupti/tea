#ifndef TOKEN_HEADER
#define TOKEN_HEADER

#include <stdlib.h>

#define TOKEN_TYPE_STR(x) #x

typedef enum TokenType {
    ON,
    OFF,
    AND,
    OR,
    GRP_OPEN,
    GRP_CLOSE,
    NOT,
    SET,
    IDENTIFIER,
    ASSIGNMENT_OPERATOR,
    STATEMENT_END,
    IF,
    THEN,
    ELSE,
    END,
    DEFINE,
    DEFINE_AS,
    DONE
} TokenType;

typedef struct Token {
    TokenType type;
    char * name;
} Token;

Token new_token(TokenType type);
Token new_identifier_token(char * name);

void print_tokens(Token * tokens, size_t tokens_len);
void print_token(Token * token);
void show_token(char * destination, Token * token);

int is_value(Token token);
int is_combinator(Token token);
int is_identifier(Token token);
int is_modifier(Token token);
int is_grp_open(Token token);
int is_grp_close(Token token);
int is_statement_end(Token token);
#endif
