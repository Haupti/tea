#include <stdio.h>
#include "syntax_verify.h"
#include "ast_node.h"
#include "token.h"

SyntaxVerification verification_with_error(SyntaxErrorType type, Token at_token, int at_position){
    SyntaxError syntax_error = { at_token, at_position, type };
    union SyntaxResult result;
    result.error = syntax_error;
    SyntaxVerification syntax_verification = { 1, result };
    return syntax_verification;
}

SyntaxVerification verification_without_error(){
    union SyntaxResult result;
    result.null = '0';
    SyntaxVerification syntax_verification = { 0, result };
    return syntax_verification;
}

void print_error_type(SyntaxErrorType type){
    switch(type){
        case SYNTX_ERR_COMBINATOR_MISSING:
            printf("SYNTX_ERR_COMBINATOR_MISSING\n");
            break;
        case SYNTX_ERR_VALUE_MISSING:
            printf("SYNTX_ERR_VALUE_MISSING\n");
            break;
        case SYNTX_ERR_UNCLOSED_GROUP:
            printf("SYNTX_ERR_UNCLOSED_GROUP\n");
            break;
        case SYNTX_ERR_NO_GROUP_TO_CLOSE:
            printf("SYNTX_ERR_NO_GROUP_TO_CLOSE\n");
            break;
        case SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP:
            printf("SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP\n");
            break;
        case SYNTX_ERR_NO_STATEMENT_BEFORE_COMBINATOR:
            printf("SYNTX_ERR_NO_STATEMENT_BEFORE_COMBINATOR\n");
            break;
        case SYNTX_ERR_NO_STATEMENT_BEFORE_GRP_CLOSE:
            printf("SYNTX_ERR_NO_STATEMENT_BEFORE_GRP_CLOSE\n");
            break;
        case SYNTX_ERR_STATEMENT_DOESNT_END:
            printf("SYNTX_ERR_STATEMENT_DOESNT_END\n");
            break;
        case SYNTX_ERR_NO_STATEMENT_TO_END:
            printf("SYNTX_ERR_NO_STATEMENT_TO_END\n");
            break;
    }
}


void print_err_and_exit(SyntaxVerification verification){
    if(verification.has_error){
        switch(verification.result.error.error_type){
            case SYNTX_ERR_NO_GROUP_TO_CLOSE:{
                    printf("SYNTAX ERROR: closing a group without matching open\n");
                    break;
                }
            case SYNTX_ERR_UNCLOSED_GROUP:{
                    printf("SYNTAX ERROR: expecting a ')'\n");
                    break;
                }
            case SYNTX_ERR_COMBINATOR_MISSING:{
                    printf("SYNTAX ERROR: missing a combinator\n");
                    break;
                }
            case SYNTX_ERR_VALUE_MISSING:{
                    printf("SYNTAX ERROR: missing a value\n");
                    break;
                }
            case SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP:{
                    printf("SYNTAX ERROR: too many tokens in this group. a group can only contain two values and one combinator\n");
                    break;
                }
            case SYNTX_ERR_NO_STATEMENT_BEFORE_COMBINATOR:{
                    printf("SYNTAX ERROR: before a combinator must be a value, there can not be a statement\n");
                    break;
                }
            case SYNTX_ERR_NO_STATEMENT_BEFORE_GRP_CLOSE:{
                    printf("SYNTAX ERROR: before closing a group must be a value, there can not be a statement\n");
                    break;
                }
            case SYNTX_ERR_STATEMENT_DOESNT_END:{
                    printf("SYNTAX ERROR: expecting a ';'\n");
                    break;
                }
            case SYNTX_ERR_NO_STATEMENT_TO_END:{
                    printf("SYNTAX ERROR: no statement to end\n");
                    break;
                }
        }
        printf("SYNTAX ERROR: at token position %d\n", verification.result.error.at_position);
        exit(EXIT_FAILURE);
    }
}

SyntaxVerification verify_syntax(Token * tokens, size_t tokens_len){
    int bracket_counter = 0;
    int statement_brackets_counter = 0;
    int consecutive_value_counter = 0;
    int consecutive_combinator_counter = 0;
    int consecutive_value_and_combinator_counter = 0;

    int i;
    Token token;
    Token * next_token;
    int next_exists = 0;
    for(i = 0; i < tokens_len; i++){
        token = tokens[i];
        if(i+1 < tokens_len){
            next_exists = 1;
            next_token = &tokens[i+1];
        }
        else {
            next_exists = 0;
            next_token = NULL;
        }

        switch(token.type){
            case ON:{
                consecutive_value_counter += 1;
                consecutive_combinator_counter = 0;
                consecutive_value_and_combinator_counter += 1;
                break;
            }
            case OFF:{
                consecutive_value_counter += 1;
                consecutive_combinator_counter = 0;
                consecutive_value_and_combinator_counter += 1;
                break;
            }
            case IDENTIFIER:{
                consecutive_value_counter += 1;
                consecutive_combinator_counter = 0;
                consecutive_value_and_combinator_counter += 1;
                break;
            }
            case AND:{
                consecutive_combinator_counter += 1;
                consecutive_value_counter = 0;
                consecutive_value_and_combinator_counter += 1;
                break;
            }
            case OR:{
                consecutive_combinator_counter += 1;
                consecutive_value_counter = 0;
                consecutive_value_and_combinator_counter += 1;
                break;
            }
            case GRP_CLOSE:{
                bracket_counter -= 1;
                consecutive_value_counter = 0;
                consecutive_combinator_counter = 0;
                consecutive_value_and_combinator_counter = 0;
                break;
            }
            case GRP_OPEN:{
                bracket_counter += 1;
                consecutive_value_counter = 0;
                consecutive_combinator_counter = 0;
                consecutive_value_and_combinator_counter = 0;
                break;
            }
            case SET:{
                statement_brackets_counter += 1;
                consecutive_value_counter = 0;
                consecutive_combinator_counter = 0;
                consecutive_value_and_combinator_counter = 0;
                break;
            }
            case STATEMENT_END:{
                statement_brackets_counter -= 1;
                consecutive_value_counter = 0;
                consecutive_combinator_counter = 0;
                consecutive_value_and_combinator_counter = 0;
                break;
            }
            case ASSIGNMENT_OPERATOR:{
                consecutive_combinator_counter = 0;
                consecutive_value_counter = 0;
                consecutive_value_and_combinator_counter = 0;
                break;
            }
            default:
                break;
        }

        if(statement_brackets_counter < 0){
            return verification_with_error(SYNTX_ERR_NO_STATEMENT_TO_END, token, i);
        }

        if(bracket_counter < 0){
            return verification_with_error(SYNTX_ERR_NO_GROUP_TO_CLOSE, token, i);
        }

        if(consecutive_value_counter > 1){
            return verification_with_error(SYNTX_ERR_COMBINATOR_MISSING, token, i);
        }

        if(consecutive_combinator_counter > 1){
            return verification_with_error(SYNTX_ERR_VALUE_MISSING, token, i);
        }

        if(consecutive_value_and_combinator_counter > 3){
            return verification_with_error(SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP, token, i);
        }

        if(!next_exists){
            continue;
        }

        if(is_statement_end(token) && is_combinator(*next_token)){
            return verification_with_error(SYNTX_ERR_NO_STATEMENT_BEFORE_COMBINATOR, *next_token, i+1);
        }
        if(is_statement_end(token) && is_grp_close(*next_token)){
            return verification_with_error(SYNTX_ERR_NO_STATEMENT_BEFORE_GRP_CLOSE, *next_token, i+1);
        }
        if(is_modifier(token) && is_combinator(*next_token)){
            return verification_with_error(SYNTX_ERR_VALUE_MISSING, *next_token, i+1);
        }
        if(is_value(token) && is_modifier(*next_token)){
            return verification_with_error(SYNTX_ERR_COMBINATOR_MISSING, *next_token, i+1);
        }
        if(token.type == GRP_OPEN && next_token->type == GRP_CLOSE){
            return verification_with_error(SYNTX_ERR_VALUE_MISSING, *next_token, i+1);
        }
    }

    if(bracket_counter > 0){
        return verification_with_error(SYNTX_ERR_UNCLOSED_GROUP, token, i);
    }

    if(statement_brackets_counter > 0){
        return verification_with_error(SYNTX_ERR_STATEMENT_DOESNT_END, token, i);
    }

    return verification_without_error();
}
