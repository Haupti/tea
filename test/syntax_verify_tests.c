#include <stdio.h>
#include "assert.h"
#include "assert_helpers.h"
#include "../src/token.h"
#include "../src/syntax_verify.h"

MODULAR_DESCRIBE(syntax_verify_tests,{
    TEST("verifies no open bracket before closing bracket",{
        Token tokens[] = ARRAY(new_token(ON), new_token(GRP_CLOSE), new_token(ON), new_token(GRP_OPEN));
        SyntaxVerification result = verify_syntax(tokens,4);
        ASSERT_INT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_NO_GROUP_TO_CLOSE);
    })
    TEST("verifies no unclosed bracket",{
        Token tokens[] = ARRAY(new_token(ON), new_token(GRP_OPEN), new_token(GRP_OPEN), new_token(ON), new_token(GRP_CLOSE));
        SyntaxVerification result = verify_syntax(tokens,5);
        ASSERT_INT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_UNCLOSED_GROUP);
    })
    TEST("verifies no error",{
        Token tokens[] = ARRAY(new_token(ON), new_token(AND) ,new_token(GRP_OPEN), new_token(GRP_OPEN), new_token(ON), new_token(GRP_CLOSE), new_token(GRP_CLOSE));
        SyntaxVerification result = verify_syntax(tokens,7);
        ASSERT_INT_EQUALS(result.has_error, 0);
    })
    TEST("verifies no empty bracket",{
        Token tokens[] = ARRAY(new_token(GRP_OPEN), new_token(GRP_CLOSE));
        SyntaxVerification result = verify_syntax(tokens,2);
        ASSERT_INT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_VALUE_MISSING);
    })
    TEST("verifies no value followed by value",{
        Token tokens[] = ARRAY(new_token(ON), new_token(OFF));
        SyntaxVerification result = verify_syntax(tokens,2);
        ASSERT_INT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_COMBINATOR_MISSING);
    })
    TEST("verifies no value followed by value even with combinator",{
        Token tokens[] = ARRAY(new_token(ON), new_token(NOT), new_token(OFF));
        SyntaxVerification result = verify_syntax(tokens,3);
        ASSERT_INT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_COMBINATOR_MISSING);
    })
    TEST("verifies no combinator followed by combinator",{
        Token tokens[] = ARRAY(new_token(AND), new_token(OR));
        SyntaxVerification result = verify_syntax(tokens,2);
        ASSERT_INT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_VALUE_MISSING);
    })
    TEST("verifies not to many elements in group",{
        Token tokens[] = ARRAY(new_token(GRP_OPEN), new_token(ON), new_token(AND), new_token(OFF), new_token(OR), new_token(ON), new_token(GRP_CLOSE));
        SyntaxVerification result = verify_syntax(tokens,7);
        ASSERT_INT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP);
    })
    TEST("verifies not to many elements top level",{
        Token tokens[] = ARRAY(new_token(ON), new_token(AND), new_token(OFF), new_token(OR), new_token(ON));
        SyntaxVerification result = verify_syntax(tokens,5);
        ASSERT_INT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP);
    })
    TEST("verifies no missing end on conditional", {
        Token tokens[] = ARRAY(new_token(IF), new_token(ON), new_token(THEN), new_token(OFF), new_token(ELSE), new_token(ON));
        SyntaxVerification result = verify_syntax(tokens,5);
        ASSERT_INT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_INCOMPLETE_CONDITIONAL);
    })
})
