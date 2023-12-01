#include "assert.h"
#include "assert_helpers.h"
#include "../src/tea.h"
#include <stdio.h>
#include <string.h>

MODULAR_DESCRIBE(acceptance_tests,{
    TEST("evaluates 1", {
        char prog[] = "1";
        Value val = tea(prog, strlen(prog));
        ASSERT_EQUALS(val, VALUE_ON);
    })
    TEST("evaluates with assignments", {
        char prog[] = "set a = 1 | 0 in\nset b = (a & 1) in\nset c = b in\n (b | a) & c\n";
        Value val = tea(prog, strlen(prog));
        ASSERT_EQUALS(val, VALUE_ON);
    })
})
