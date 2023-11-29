#include "assert.h"
#include "assert_helpers.h"
#include <stdio.h>
#include "../src/sanity_check.h"
#include "../src/token.h"
#include <string.h>

MODULAR_DESCRIBE(sanity_check_tests,{
    TEST("checks that assignments dont reference itself",{
        char * a = "a";
        Token tokens[] = ARRAY(new_token(SET), new_identifier_token(a), new_token(ASSIGNMENT_OPERATOR), new_identifier_token(a), new_token(STATEMENT_END));
        SanityCheck check = sanity_check(tokens, 5);
        ASSERT_INT_EQUALS(check.has_error, 1);
        ASSERT_STR_EQUALS(SANITY_ERROR_TYPE_STR(check.result.error.error_type),SANITY_ERROR_TYPE_STR(SNITY_ERR_USE_BEFORE_ASSIGNMENT));
    })
    TEST("checks that assignments doesnt 'shadow' the identifier to be assigned",{
        char * a = "a";
        Token tokens[] = ARRAY(new_token(SET), new_identifier_token(a), new_token(ASSIGNMENT_OPERATOR),
                               new_token(SET), new_identifier_token(a), new_token(ASSIGNMENT_OPERATOR),
                               new_token(ON), new_token(STATEMENT_END), new_identifier_token(a),
                               new_token(STATEMENT_END), new_identifier_token(a));
        SanityCheck check = sanity_check(tokens, 11);
        ASSERT_INT_EQUALS(check.has_error, 1);
        ASSERT_STR_EQUALS(SANITY_ERROR_TYPE_STR(check.result.error.error_type),SANITY_ERROR_TYPE_STR(SNITY_ERR_NO_OVERWRITE));
    })
    TEST("checks that assignments dont overwrite existing identifier (not within assignment itself)",{
        char * a = "a";
        Token tokens[] = ARRAY(new_token(SET), new_identifier_token(a), new_token(ASSIGNMENT_OPERATOR), new_token(ON), new_token(STATEMENT_END),
                               new_token(SET), new_identifier_token(a), new_token(ASSIGNMENT_OPERATOR), new_token(ON), new_token(STATEMENT_END),
                               new_identifier_token(a));
        SanityCheck check = sanity_check(tokens, 11);
        ASSERT_INT_EQUALS(check.has_error, 1);
        ASSERT_STR_EQUALS(SANITY_ERROR_TYPE_STR(check.result.error.error_type),SANITY_ERROR_TYPE_STR(SNITY_ERR_NO_OVERWRITE));
    })
    TEST("checks you cannot use an identifier before assignment before",{
        char * a = "a";
        Token tokens[] = ARRAY(new_identifier_token(a),
                               new_token(SET), new_identifier_token(a), new_token(ASSIGNMENT_OPERATOR), new_token(ON), new_token(STATEMENT_END));
        SanityCheck check = sanity_check(tokens, 6);
        ASSERT_INT_EQUALS(check.has_error, 1);
        ASSERT_STR_EQUALS(SANITY_ERROR_TYPE_STR(check.result.error.error_type),SANITY_ERROR_TYPE_STR(SNITY_ERR_USE_BEFORE_ASSIGNMENT));
    })
})
