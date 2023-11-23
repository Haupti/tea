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
                    Token token = {GRP_OPEN};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case ')':
                {
                    Token token = {GRP_CLOSE};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '1':
                {
                    Token token = {ON};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '0':
                {
                    Token token = {OFF};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
                token_index += 1;
            case '&':
                {
                    Token token = {AND};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '|':
                {
                    Token token = {OR};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '!':
                {
                    Token token = {NOT};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
        }
    }
    return token_index;
}
