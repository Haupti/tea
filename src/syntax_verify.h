#ifndef SYNTAX_VERIFY_HEADER
#define SYNTAX_VERIFY_HEADER

#include "token.h"
#include "cursor.h"

typedef enum SyntaxErrorType {
    UNCLOSED_GROUP,
    NO_GROUP_TO_CLOSE,
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

#endif
