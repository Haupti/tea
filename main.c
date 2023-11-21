#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include "src/ast_node.h"
#include "src/lexer.h"
#include "src/ast_builder.h"
#include "src/ast_evaluate.h"

int main(){
    char * program = "( 0 & ((((1 & 0)) & 1) | 1)) | 1";
    Token * tokens = malloc(sizeof(Token) * 10000);
    int tokens_len = read_tokens(tokens, program, strlen(program));

    tokens = realloc(tokens, sizeof(Token) * tokens_len);
    Node tree = build_tree(tokens, 0, tokens_len-1);
    Value val = evaluate_node(&tree);
    switch(val){
        case VALUE_ON:
            puts("1");
            break;
        case VALUE_OFF:
            puts("0");
            break;
    }
    return EXIT_SUCCESS;
}

