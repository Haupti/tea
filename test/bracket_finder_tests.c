#include "assert.h"
#include "assert_helpers.h"
#include <stdio.h>
#include "../src/token.h"
#include "../src/bracket_finder.h"

MODULAR_DESCRIBE(bracket_finder_tests,{
    TEST("find body of group",{
        Token tokens[] = ARRAY(new_token(GRP_OPEN), new_token(GRP_OPEN), new_token(ON), new_token(GRP_CLOSE), new_token(GRP_CLOSE));
        Slice slice = new_slice(tokens, 0, LEN(tokens));
        Slice body = find_bracket_slice(slice);
        ASSERT_INT_EQUALS(body.start, 1);
        ASSERT_INT_EQUALS(body.end, 3);
    })
    TEST("find body of assignment",{
        Token tokens[] = ARRAY(new_token(SET), new_token(IDENTIFIER), new_token(ASSIGNMENT_OPERATOR), new_token(ON), new_token(STATEMENT_END));
        Slice slice = new_slice(tokens, 0, LEN(tokens));
        Slice body = find_bracket_slice(slice);
        ASSERT_INT_EQUALS(body.start, 1);
        ASSERT_INT_EQUALS(body.end, 3);
    })
    TEST("find body of if - then",{
        Token tokens[] = ARRAY(new_token(IF), new_token(ON), new_token(THEN), new_token(OFF), new_token(ELSE), new_token(OFF), new_token(END));
        Slice slice = new_slice(tokens, 0, LEN(tokens));
        Slice body = find_bracket_slice(slice);
        ASSERT_INT_EQUALS(body.start, 1);
        ASSERT_INT_EQUALS(body.end, 1);
    })
    TEST("find body of then - else",{
        Token tokens[] = ARRAY(new_token(IF), new_token(ON), new_token(THEN), new_token(OFF), new_token(ELSE), new_token(OFF), new_token(END));
        Slice slice = new_slice(tokens, 2, LEN(tokens));
        Slice body = find_bracket_slice(slice);
        ASSERT_INT_EQUALS(body.start, 3);
        ASSERT_INT_EQUALS(body.end, 3);
    })
    TEST("find body of else - end",{
        Token tokens[] = ARRAY(new_token(IF), new_token(ON), new_token(THEN), new_token(OFF), new_token(ELSE), new_token(OFF), new_token(END));
        Slice slice = new_slice(tokens, 4, LEN(tokens));
        Slice body = find_bracket_slice(slice);
        ASSERT_INT_EQUALS(body.start, 5);
        ASSERT_INT_EQUALS(body.end, 5);
    })
})
