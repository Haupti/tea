#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"

#include "../src/lexer.h"
#include "../src/ast_builder.h"
#include "../src/token.h"
#include "../src/ast_evaluate.h"

DESCRIBE("read_tokens", {
    TEST("reads 1 & 0",{
        Token tokens[5];
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
    TEST("reads (1 & 0) | 1)",{
        Token tokens[5];
        int count = read_tokens(tokens, "(1 & 0) | 1", 11);
        ASSERT_EQUALS(count,7);
        char show[20];
        show_token(show, &tokens[0]);
        ASSERT_STR_EQUALS(show,"GRP_OPEN");
        show_token(show, &tokens[4]);
        ASSERT_STR_EQUALS(show,"GRP_CLOSE");
        show_token(show, &tokens[5]);
        ASSERT_STR_EQUALS(show,"OR");
    })
    XTEST("prints leaf",{
        Node node = new_leaf(VALUE_ON);
        print_tree(node);
    })
    XTEST("prints node",{
        Node left = new_leaf(VALUE_ON);
        Node right = new_leaf(VALUE_OFF);
        Node node = new_node(&left, &right, COMBINATOR_AND);
        print_tree(node);
    })
    TEST("evaluates leaf",{
        Node node = new_leaf(VALUE_ON);
        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("evaluates flat fork",{
        Node left = new_leaf(VALUE_ON);
        Node right = new_leaf(VALUE_OFF);
        Node node = new_node(&left, &right, COMBINATOR_AND);
        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_OFF);
    })
    TEST("evaluates nested fork (left is leaf)",{
        Node left = new_leaf(VALUE_ON);

        Node left_d1 = new_leaf(VALUE_OFF);
        Node right_d1 = new_leaf(VALUE_ON);

        Node node_d1 = new_node(&left_d1, &right_d1, COMBINATOR_OR);

        Node node = new_node(&left, &node_d1, COMBINATOR_AND);

        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("evaluates nested fork (right is leaf)",{
        Node right = new_leaf(VALUE_ON);

        Node left_d1 = new_leaf(VALUE_ON);
        Node right_d1 = new_leaf(VALUE_ON);

        Node node_d1 = new_node(&left_d1, &right_d1, COMBINATOR_AND);

        Node node = new_node(&node_d1, &right, COMBINATOR_AND);

        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_ON);
    })
})
