#include <io.h>
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
    if(syntax_verification.has_error){
        switch(syntax_verification.result.error.error_type){
            case NO_GROUP_TO_CLOSE:{
                    printf("SYNTAX ERROR: closing a group without matching open\n");
                    printf("SYNTAX ERROR: at token position %d\n", syntax_verification.result.error.at_position);
                    exit(EXIT_FAILURE);
                    break;
                }
            case UNCLOSED_GROUP:{
                    printf("SYNTAX ERROR: expecting a ')'\n");
                    printf("SYNTAX ERROR: at token position %d\n", syntax_verification.result.error.at_position);
                    exit(EXIT_FAILURE);
                    break;
                }
        }
    }


    // build AST / binary tree
    Node tree = build_tree(tokens, 0, tokens_len-1);

    // evaluate program
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

