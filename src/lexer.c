#include <stdlib.h>
#include "token.h"
#include <stdio.h>
#include <string.h>

#include "err.h"

int is_word_start(char c){
    return (65 <= c && c <= 90) || c == 95 || (97 <= c && c <= 122);
}

int is_number(char c){
    return (48 <= c && c <= 57);
}

int is_allowed_in_word(char c){
    return is_word_start(c) || is_number(c);
}

int is_word_ending(char c){
    char is_whitespace = 9 == c || 10 == c || 32 == c;
    char is_bracket = c == 40 || c == 41 || c == 91 || c == 93 || c == 123 || c == 125;
    char is_quote = c == 96 || c == 34 || c == 39;
    char is_string_terminator = c == 0;
    return is_bracket || is_whitespace || is_quote || is_string_terminator;
}

typedef enum ReadMode {
    DEFAULT,
    WORD,
    COMMENT,
} ReadMode;

int read_tokens(Token * tokens, char * input, size_t input_len){
    char word[100] = {'\0'};
    char word_index = 0;

    ReadMode read_mode = DEFAULT;

    char comment_counter = 0;

    int token_index = 0;

    int i;
    for(i = 0; i < input_len; i++){
        char c = input[i];

        if(read_mode == COMMENT){
            if(c == '\n'){
                read_mode = DEFAULT;
                comment_counter = 0;
            }
        }
        if(read_mode == WORD){
            if(is_allowed_in_word(c)){
                word[word_index] = c;
                word_index += 1;
            }
            else if(is_word_ending(c)){
                word[word_index] = '\0';
                word_index += 1;
                if(strcmp(word, "set") == 0){
                    tokens[token_index] = new_token(SET);
                    token_index += 1;
                }
                else{
                    char * name = malloc(sizeof(char) * (word_index));
                    strcpy(name, word);
                    word_index = 0;
                    tokens[token_index] = new_identifier_token(IDENTIFIER, name);
                    token_index += 1;
                }
                read_mode = DEFAULT;
            }
            else {
                err_at("only letters, numbers and underscores are allowed in an identifier", i);
            }
        }

        if(read_mode != DEFAULT){
            continue;
        }
        switch(c){
            case '(':
                {
                    Token token = new_token(GRP_OPEN);
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case ')':
                {
                    Token token = new_token(GRP_CLOSE);
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '1':
                {
                    Token token = new_token(ON);
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '0':
                {
                    Token token = new_token(OFF);
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
                token_index += 1;
            case '&':
                {
                    Token token = new_token(AND);
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '|':
                {
                    Token token = new_token(OR);
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '!':
                {
                    Token token = new_token(NOT);
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '-':
                {
                    comment_counter += 1;
                    if(comment_counter == 2){
                        read_mode = COMMENT;
                    }
                    break;
                }
            case ';':
                {
                    Token token = {STATEMENT_END};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case '=':
                {
                    Token token = {ASSIGNMENT_OPERATOR};
                    tokens[token_index] = token;
                    token_index += 1;
                    break;
                }
            case ' ':
                break;
            case '\n':
                break;
            case '\0':
                break;
            default:
                {
                    if(is_word_start(c)){
                        word[0] = c;
                        word_index = 1;
                        read_mode = WORD;
                    }
                    else {
                        err_at("unexpected character", i);
                    }
                }

        }
    }
    return token_index;
}
