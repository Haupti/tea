#include <stdlib.h>
#include "token.h"
#include <stdio.h>

int read_tokens(Token * tokens, char * input, size_t input_len){
    int token_index = 0;
    int i;
    for(i = 0; i < input_len; i++){
        char c = input[i];
        switch(input[i]){
            case '(':
                {
                    Token token = {GRP_OPEN, NULL};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case ')':
                {
                    Token token = {GRP_CLOSE, NULL};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '1':
                {
                    Token token = {ON, NULL};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '0':
                {
                    Token token = {OFF, NULL};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
                token_index += 1;
            case '&':
                {
                    Token token = {AND, NULL};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '|':
                {
                    Token token = {OR, NULL};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
        }
    }
    return token_index;
}
