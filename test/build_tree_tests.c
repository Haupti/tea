#include <stdio.h>
#include "../src/ast_node.h"
#include "../src/token.h"
#include "../src/ast_builder.h"
#include "assert.h"
#include "assert_helpers.h"
#include <string.h>

MODULAR_DESCRIBE(build_tree_tests,{
    TEST("builds leaf", {
        Token tokens[] = ARRAY(new_token(NOT), new_token(NOT), new_token(ON));
        Node node =  build_tree(tokens,0, 2);
        ASSERT_EQUALS(node.type, LEAF);
        ASSERT_EQUALS(node.it.leaf.value, VALUE_ON);
    })
    TEST("builds sprout with leaf", {
        Token tokens[] = ARRAY(new_token(NOT), new_token(ON));
        Node node =  build_tree(tokens, 0, 1);
        ASSERT_EQUALS(node.type, SPROUT);
        ASSERT_EQUALS(node.it.sprout.modifier, MODIFIER_NOT);
        ASSERT_EQUALS(node.it.sprout.tip->type, LEAF);
        ASSERT_EQUALS(node.it.sprout.tip->it.leaf.value, VALUE_ON);
    })
    TEST("builds fork", {
        Token tokens[] = ARRAY(new_token(ON), new_token(OR), new_token(OFF));
        Node node =  build_tree(tokens, 0, 2);
        ASSERT_EQUALS(node.type, FORK);
        ASSERT_EQUALS(node.it.fork.combinator, COMBINATOR_OR);
        ASSERT_EQUALS(node.it.fork.left->type, LEAF);
        ASSERT_EQUALS(node.it.fork.left->it.leaf.value, VALUE_ON);
        ASSERT_EQUALS(node.it.fork.right->type, LEAF);
        ASSERT_EQUALS(node.it.fork.right->it.leaf.value, VALUE_OFF);
    })
    TEST("builds fork with group brackets", {
        Token tokens[] = ARRAY(new_token(GRP_OPEN), new_token(ON), new_token(OR), new_token(OFF), new_token(GRP_CLOSE));
        Node node =  build_tree(tokens, 0, 4);
        ASSERT_EQUALS(node.type, FORK);
        ASSERT_EQUALS(node.it.fork.combinator, COMBINATOR_OR);
        ASSERT_EQUALS(node.it.fork.left->type, LEAF);
        ASSERT_EQUALS(node.it.fork.left->it.leaf.value, VALUE_ON);
        ASSERT_EQUALS(node.it.fork.right->type, LEAF);
        ASSERT_EQUALS(node.it.fork.right->it.leaf.value, VALUE_OFF);
    })
    TEST("builds conditional node", {
        Token tokens[] = ARRAY(new_token(IF), new_token(ON), new_token(THEN), new_token(ON), new_token(ELSE), new_token(OFF), new_token(END));
        Node node =  build_tree(tokens, 0, LEN(tokens));
        ASSERT_EQUALS(node.type, CONDITIONAL);
        ASSERT_EQUALS(node.it.conditional.condition->type, LEAF);
        ASSERT_EQUALS(node.it.conditional.then->type, LEAF);
        ASSERT_EQUALS(node.it.conditional.otherwise->type, LEAF);
    })
    TEST("builds conditional node after assignment", {
        Token tokens[] = ARRAY(new_token(SET), new_identifier_token("A"), new_token(ASSIGNMENT_OPERATOR), new_token(ON), new_token(STATEMENT_END),
            new_token(IF), new_identifier_token("A"), new_token(THEN), new_token(ON), new_token(ELSE), new_token(OFF), new_token(END));
        Node node =  build_tree(tokens, 0, LEN(tokens));
        ASSERT_EQUALS(node.type, CONDITIONAL);
        ASSERT_EQUALS(node.it.conditional.condition->type, IDENTIFIER_LEAF);
        ASSERT_EQUALS(node.it.conditional.then->type, LEAF);
        ASSERT_EQUALS(node.it.conditional.otherwise->type, LEAF);
    })
    TEST("builds function definition and call", {
        Token tokens[] = ARRAY(new_token(DEFINE), new_identifier_token("funcname"), new_identifier_token("a"), new_identifier_token("b"), new_token(DEFINE_AS),
            new_identifier_token("a"), new_token(OR), new_identifier_token("b"), new_token(DONE), new_identifier_token("funcname"), new_token(ON), new_token(OFF));
        Node node =  build_tree(tokens, 0, LEN(tokens));
        // node itself
        ASSERT_EQUALS(node.type, FUNCTION_CALL_NODE);
        ASSERT_STR_EQUALS(node.it.function_call.function_identifier, "funcname");
        ASSERT_INT_EQUALS(node.it.function_call.params_count, 2);
        ASSERT_EQUALS(node.it.function_call.params[0]->type, LEAF);
        ASSERT_EQUALS(node.it.function_call.params[1]->type, LEAF);

        // function
        ASSERT_INT_EQUALS(node.it.function_call.functions_count, 1);
        Function function = *node.it.function_call.functions[0];
        ASSERT_STR_EQUALS(function.function_identifier, "funcname");
        ASSERT_INT_EQUALS(function.args_count, 2);
        ASSERT_STR_EQUALS(function.arg_names[0], "a");
        ASSERT_STR_EQUALS(function.arg_names[0], "b");
        ASSERT_EQUALS(function.body->type, FORK);
        ASSERT_EQUALS(function.body->it.fork.left->type, FUNCTION_PARAM_LEAF);
        ASSERT_STR_EQUALS(function.body->it.fork.left->it.fn_param_leaf.name, "a");
        ASSERT_EQUALS(function.body->it.fork.right->type, FUNCTION_PARAM_LEAF);
        ASSERT_STR_EQUALS(function.body->it.fork.right->it.fn_param_leaf.name, "b");
    })
})
