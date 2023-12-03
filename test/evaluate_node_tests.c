
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "assert_helpers.h"

#include "../src/slice.h"
#include "../src/lexer.h"
#include "../src/ast_builder.h"
#include "../src/token.h"
#include "../src/ast_evaluate.h"
#include "../src/syntax_verify.h"
#include "assert_helpers.h"

Node new_top_level_leaf(Value value){
    return new_leaf(value);
}

ParamScope empty = { NULL, 0};

MODULAR_DESCRIBE(evaluate_node_tests, {
    TEST("evaluates leaf",{
        Node node = new_top_level_leaf(VALUE_ON);
        Value value = evaluate_node(&node, empty);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("evaluates flat fork",{
        Node left = new_top_level_leaf(VALUE_ON);
        Node right = new_top_level_leaf(VALUE_OFF);
        Node node = new_fork(&left, &right, COMBINATOR_AND);
        Value value = evaluate_node(&node, empty);
        ASSERT_EQUALS(value, VALUE_OFF);
    })
    TEST("evaluates nested fork (left is leaf)",{
        Node left = new_top_level_leaf(VALUE_ON);

        Node left_d1 = new_top_level_leaf(VALUE_OFF);
        Node right_d1 = new_top_level_leaf(VALUE_ON);

        Node node_d1 = new_fork(&left_d1, &right_d1, COMBINATOR_OR);

        Node node = new_fork(&left, &node_d1, COMBINATOR_AND);

        Value value = evaluate_node(&node, empty);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("evaluates nested fork (right is leaf)",{
        Node right = new_top_level_leaf(VALUE_ON);

        Node left_d1 = new_top_level_leaf(VALUE_ON);
        Node right_d1 = new_top_level_leaf(VALUE_ON);

        Node node_d1 = new_fork(&left_d1, &right_d1, COMBINATOR_AND);

        Node node = new_fork(&node_d1, &right, COMBINATOR_AND);

        Value value = evaluate_node(&node, empty);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("evaluates conditional",{
        Node condition = new_top_level_leaf(VALUE_ON);

        Node then = new_top_level_leaf(VALUE_OFF);
        Node otherwise = new_top_level_leaf(VALUE_ON);

        Node conditional = new_conditional(&condition, &then, &otherwise);

        Value value = evaluate_node(&conditional, empty);
        ASSERT_EQUALS(value, VALUE_OFF);
    })
})




















