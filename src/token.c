#include "token.h"
#include <stdio.h>
#include <string.h>

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
    char token_representation[10];
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
    }
}
