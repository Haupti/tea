#ifndef SANITY_CHECK_HEADER
#define SANITY_CHECK_HEADER

#include "token.h"
#include "slice.h"

#define SANITY_ERROR_TYPE_STR(x) (x == SNITY_ERR_USE_BEFORE_ASSIGNMENT ? "SNITY_ERR_USE_BEFORE_ASSIGNMENT" :\
(SNITY_ERR_NO_OVERWRITE ? "SNITY_ERR_NO_OVERWRITE" : ""))

typedef enum SanityErrorType {
    SNITY_ERR_USE_BEFORE_ASSIGNMENT,
    SNITY_ERR_NO_OVERWRITE,
} SanityErrorType;

typedef struct SanityError {
    Token at_token;
    int at_position;
    SanityErrorType error_type;
} SanityError;

union SanityCheckResult {
    SanityError error;
    char null;
};

typedef struct SanityCheck {
    int has_error;
    union SanityCheckResult result;
} SanityCheck;

SanityCheck sanity_check(Token * tokens, size_t tokens_len);

void print_snty_err_and_exit(SanityCheck snty_check);
#endif
