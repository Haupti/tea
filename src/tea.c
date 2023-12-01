#include "lexer.h"
#include "sanity_check.h"
#include "syntax_verify.h"
#include "ast_builder.h"
#include "ast_evaluate.h"

#include <stdio.h>
#include <string.h>

Value tea(char * program, int program_size){

    // lexing
    Token * tokens = malloc(sizeof(Token) * program_size); // if every char would result in a token
    int tokens_len = read_tokens(tokens, program, strlen(program));
    tokens = realloc(tokens, sizeof(Token) * tokens_len); // it propably didnt -> resize

    // syntax check
    SyntaxVerification syntax_verification = verify_syntax(tokens, tokens_len);
    print_syntx_err_and_exit(syntax_verification);

    // sanity check
    SanityCheck snty_check = sanity_check(tokens, tokens_len);
    print_snty_err_and_exit(snty_check);

    // build AST / binary tree
    Node tree = build_tree(tokens, 0, tokens_len-1);
    printf("DEBUG: tree:\n");
    print_tree(tree);
    printf("\n");

    // evaluate program and return
    return evaluate_node(&tree);
}
