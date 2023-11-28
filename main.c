#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/ast_node.h"
#include "src/fileio.h"
#include "src/lexer.h"
#include "src/ast_builder.h"
#include "src/ast_evaluate.h"
#include "src/syntax_verify.h"
#include "src/token.h"
#include "src/err.h"

int main(int args, char * argv[]){

    puts((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__));
    return EXIT_SUCCESS; // TODO
    if(args != 2){
        puts("expected exaclty one argument (filename)");
        return EXIT_FAILURE;
    }

    // read file
    FileContent file_content = read_file(argv[1]);
    long filesize = file_content.size;
    char * program = file_content.buffer;

    // lexing
    Token * tokens = malloc(sizeof(Token) * filesize); // if every char would result in a token
    int tokens_len = read_tokens(tokens, program, strlen(program));
    tokens = realloc(tokens, sizeof(Token) * tokens_len); // it propably didnt -> resize

    // syntax check
    SyntaxVerification syntax_verification = verify_syntax(tokens, tokens_len);
    print_err_and_exit(syntax_verification);


    // build AST / binary tree
    Node tree = build_tree(tokens, 0, tokens_len-1);
    print_tree(tree);

    // evaluate program
    Value val = evaluate_node(&tree);
;
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

