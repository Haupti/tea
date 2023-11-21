#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

int read_tokens(TokenType * tokens, char * input, size_t input_len){
    int token_index = 0;
    int i;
    for(i = 0; i < input_len; i++){
        switch(input[i]){
            case '(':
                tokens[token_index] = GRP_OPEN;
                token_index += 1;
                break;
            case ')':
                tokens[token_index] = GRP_CLOSE;
                token_index += 1;
                break;
            case '1':
                tokens[token_index] = ON;
                token_index += 1;
                break;
            case '0':
                tokens[token_index] = OFF;
                token_index += 1;
                break;
            case '&':
                tokens[token_index] = AND;
                token_index += 1;
                break;
            case '|':
                tokens[token_index] = OR;
                token_index += 1;
                break;
        }
    }
    return token_index;
}

void print_token(TokenType * token){
    char token_representation[10];
    show_token(token_representation, token);
    printf("%s\n", token_representation);
}


void show_token(char * destination, TokenType * token){
    switch(*token){
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
