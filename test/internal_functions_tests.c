#include "assert.h"
#include "assert_helpers.h"
#include "ast_builder_internal.h"
#include "../src/slice.h"
#include "../src/token.h"
#include <stdio.h>


MODULAR_DESCRIBE(internal_functions_tests,{
    TEST("cuts group slice to size (cuts off end after group close and the brackets)", {
        Token tokens[] = ARRAY(new_token(GRP_OPEN), new_token(ON), new_token(GRP_CLOSE), new_token(ON));
        Slice slice = new_slice(tokens, 0, 3);
        Slice grou_slice = cut_group_slice_to_size(slice);

        ASSERT_INT_EQUALS(grou_slice.start, 1);
        ASSERT_INT_EQUALS(grou_slice.end, 1);
    })
})
