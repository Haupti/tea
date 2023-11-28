#include <stdio.h>
#include "../src/ast_node.h"
#include "../src/token.h"
#include "../src/ast_builder.h"
#include "assert.h"
#include "assert_helpers.h"

MODULAR_DESCRIBE(build_tree_tests,{
    TEST("builds leaf", {
        Token tokens[] = ARRAY(new_token(NOT), new_token(NOT), new_token(ON));
        Node node =  build_tree(tokens,0, 2);
        ASSERT_EQUALS(node.type, LEAF);
        ASSERT_EQUALS(node.value.value, VALUE_ON);
    })
    TEST("builds sprout with leaf", {
        Token tokens[] = ARRAY(new_token(NOT), new_token(ON));
        Node node =  build_tree(tokens, 0, 1);
        ASSERT_EQUALS(node.type, SPROUT);
        ASSERT_EQUALS(node.value.modifier, MODIFIER_NOT);
        ASSERT_EQUALS(node.tip->type, LEAF);
        ASSERT_EQUALS(node.tip->value.value, VALUE_ON);
    })
    TEST("builds fork", {
        Token tokens[] = ARRAY(new_token(ON), new_token(OR), new_token(OFF));
        Node node =  build_tree(tokens, 0, 2);
        ASSERT_EQUALS(node.type, FORK);
        ASSERT_EQUALS(node.value.combinator, COMBINATOR_OR);
        ASSERT_EQUALS(node.left->type, LEAF);
        ASSERT_EQUALS(node.left->value.value, VALUE_ON);
        ASSERT_EQUALS(node.right->type, LEAF);
        ASSERT_EQUALS(node.right->value.value, VALUE_OFF);
    })
    TEST("builds fork with group brackets", {
        Token tokens[] = ARRAY(new_token(GRP_OPEN), new_token(ON), new_token(OR), new_token(OFF), new_token(GRP_CLOSE));
        Node node =  build_tree(tokens, 0, 4);
        ASSERT_EQUALS(node.type, FORK);
        ASSERT_EQUALS(node.value.combinator, COMBINATOR_OR);
        ASSERT_EQUALS(node.left->type, LEAF);
        ASSERT_EQUALS(node.left->value.value, VALUE_ON);
        ASSERT_EQUALS(node.right->type, LEAF);
        ASSERT_EQUALS(node.right->value.value, VALUE_OFF);
    })
})
