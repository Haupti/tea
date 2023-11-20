#include "assert.h"
#include <stdio.h>
#include <stdlib.h>

DESCRIBE(tea, {
    TEST("aa bb",{

        int i = 2;
        ASSERT_EQUALS(i,3);
    })
    TEST("aa bb",{
        ASSERT_EQUALS(1, 1);
    })
})
