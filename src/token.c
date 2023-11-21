#include "token.h"
#include <stdio.h>
#include <string.h>

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
    }
}
