#include "syntax_verify.h"
#include "cursor.h"
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

SyntaxVerification verify_syntax(Token * tokens, size_t tokens_len){
    int bracket_counter = 0;

    print_tokens(tokens, tokens_len);
    int i;
    Token token;
    for(i = 0; i < tokens_len; i++){
        token = tokens[i];
        switch(token.type){
            case GRP_OPEN: {
                    bracket_counter += 1;
                    break;
                }
            case GRP_CLOSE: {
                    bracket_counter -= 1;
                    break;
                }
            default:
                break;
        }

        if(bracket_counter < 0){
            return verification_with_error(NO_GROUP_TO_CLOSE, token, i);
        }
    }

    if(bracket_counter > 0){
        return verification_with_error(UNCLOSED_GROUP, token, i);
    }

    return verification_without_error();
}
