#ifndef SYNTAX_VERIFY_HEADER
#define SYNTAX_VERIFY_HEADER

#include "token.h"

typedef enum SyntaxErrorType {
    SYNTX_ERR_UNCLOSED_GROUP,
    SYNTX_ERR_NO_GROUP_TO_CLOSE,
    SYNTX_ERR_VALUE_MISSING,
    SYNTX_ERR_COMBINATOR_MISSING,
    SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP,
} SyntaxErrorType;

typedef struct SyntaxError {
    Token at_token;
    int at_position;
    SyntaxErrorType error_type;
} SyntaxError;

union SyntaxResult {
    SyntaxError error;
    char null;
};

typedef struct SyntaxVerification {
    int has_error;
    union SyntaxResult result;
} SyntaxVerification;

SyntaxVerification verify_syntax(Token * tokens, size_t tokens_len);

// does not exit if it has no error
void print_err_and_exit(SyntaxVerification verification);

void print_error_type(SyntaxErrorType type);
#endif
