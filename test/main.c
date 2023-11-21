#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"

#include "../src/lexer.h"

DESCRIBE("read_tokens", {
    TEST("reads 1 & 0",{
        TokenType tokens[5];
        int count = read_tokens(tokens, "1 & 0", 5);
        ASSERT_EQUALS(count,3);
        char show[20];
        show_token(show, &tokens[0]);
        ASSERT_STR_EQUALS(show,"ON");
        show_token(show, &tokens[1]);
        ASSERT_STR_EQUALS(show,"AND");
        show_token(show, &tokens[2]);
        ASSERT_STR_EQUALS(show,"OFF");
    })
})
