#include "token.h"
#include <stdio.h>
#include <string.h>

Token new_token(TokenType type){
    Token token = {type, NULL};
    return token;
}

Token new_identifier_token(TokenType type, char * name){
    Token token = {type, name};
    return token;
}

int is_value(Token token){
    return token.type == ON || token.type == OFF;
}

int is_combinator(Token token){
    return token.type == AND || token.type == OR;
}

int is_modifier(Token token){
    return token.type == NOT;
}

int is_grp_open(Token token){
    return token.type == GRP_OPEN;
}

void print_tokens(Token * tokens, size_t tokens_len){
    for(int i = 0; i < tokens_len; i++){
        print_token(&tokens[i]);
    }
}

void print_token(Token * token){
    char token_representation[42];
    show_token(token_representation, token);
    printf("%s\n", token_representation);
}


void show_token(char * destination, Token * token){
    switch(token->type){
        case GRP_CLOSE:
            strcpy(destination, "GRP_CLOSE");
            break;
        case GRP_OPEN:
            strcpy(destination, "GRP_OPEN");
            break;
        case AND:
            strcpy(destination, "AND");
            break;
        case OR:
            strcpy(destination, "OR");
            break;
        case ON:
            strcpy(destination, "ON");
            break;
        case OFF:
            strcpy(destination, "OFF");
            break;
        case NOT:
            strcpy(destination, "NOT");
            break;
        case SET:
            strcpy(destination, "SET");
            break;
        case IDENTIFIER:
            {
                char s[42];
                if(strlen(token->name) > 29){
                    snprintf(s, 29, "IDENTIFIER(%s...)", token->name);
                }
                else {
                    snprintf(s, 29, "IDENTIFIER(%s)", token->name);
                }
                strcpy(destination, s);
                break;
            }
        case ASSIGNMENT_OPERATOR:
            strcpy(destination, "ASSIGNMENT_OPERATOR");
            break;
        case STATEMENT_END:
            strcpy(destination, "STATEMENT_END");
            break;
    }
}
