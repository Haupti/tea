#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/ast_node.h"
#include "src/fileio.h"
#include "src/lexer.h"
#include "src/ast_builder.h"
#include "src/ast_evaluate.h"

int main(int args, char * argv[]){

    if(args != 2){
        puts("expected exaclty one argument (filename)");
        return EXIT_FAILURE;
    }

    int filesize = file_size(argv[1]);
    char * program = malloc(sizeof(char) * (filesize + 1));
    read_file(argv[1],program, filesize);

    Token * tokens = malloc(sizeof(Token) * filesize); // if every char would result in a token
    int tokens_len = read_tokens(tokens, program, strlen(program));
    tokens = realloc(tokens, sizeof(Token) * tokens_len); // it propably didnt -> resize

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

