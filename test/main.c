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
#include "ast_builder_internal.h"


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
    TEST("evaluates leaf",{
        Node node = new_leaf(VALUE_ON);
        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("evaluates flat fork",{
        Node left = new_leaf(VALUE_ON);
        Node right = new_leaf(VALUE_OFF);
        Node node = new_fork(&left, &right, COMBINATOR_AND);
        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_OFF);
    })
    TEST("evaluates nested fork (left is leaf)",{
        Node left = new_leaf(VALUE_ON);

        Node left_d1 = new_leaf(VALUE_OFF);
        Node right_d1 = new_leaf(VALUE_ON);

        Node node_d1 = new_fork(&left_d1, &right_d1, COMBINATOR_OR);

        Node node = new_fork(&left, &node_d1, COMBINATOR_AND);

        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("evaluates nested fork (right is leaf)",{
        Node right = new_leaf(VALUE_ON);

        Node left_d1 = new_leaf(VALUE_ON);
        Node right_d1 = new_leaf(VALUE_ON);

        Node node_d1 = new_fork(&left_d1, &right_d1, COMBINATOR_AND);

        Node node = new_fork(&node_d1, &right, COMBINATOR_AND);

        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("correctly finds left expression",{
        Token test_expression_fork[] = ARRAY({ NOT }, {ON}, {OR}, {NOT}, {GRP_OPEN}, {OFF}, {GRP_CLOSE});
        Cursor cursor = new_cursor(test_expression_fork,0,7);
        Cursor left_cursor = left_expression_cursor(&cursor);
        ASSERT_EQUALS(left_cursor.position, 0);
        ASSERT_EQUALS(current(&left_cursor).type, NOT);
        ASSERT_EQUALS(next(&left_cursor).type, ON);
        ASSERT_EQUALS(has_next(&left_cursor), 0);
    })
    TEST("correctly finds right expression",{
        Token test_expression_fork[] = ARRAY({ NOT }, {ON}, {OR}, {NOT}, {GRP_OPEN}, {OFF}, {GRP_CLOSE});
        Cursor cursor = new_cursor(test_expression_fork,0,7);
        Cursor right_cursor = right_expression_cursor(&cursor);
        ASSERT_EQUALS(right_cursor.position, 3);
        ASSERT_EQUALS(current(&right_cursor).type, NOT);
        ASSERT_EQUALS(next(&right_cursor).type, GRP_OPEN);
        ASSERT_EQUALS(next(&right_cursor).type, OFF);
        ASSERT_EQUALS(next(&right_cursor).type, GRP_CLOSE);
        ASSERT_EQUALS(has_next(&right_cursor), 0);
    })
    TEST("correctly finds combinator", {
        Token test_expression_fork[] = ARRAY({ NOT }, {ON}, {OR}, {NOT}, {GRP_OPEN}, {OFF}, {GRP_CLOSE});
        Cursor cursor = new_cursor(test_expression_fork,0,7);
        int combinator_pos = find_matching_combinator(&cursor);
        ASSERT_EQUALS(combinator_pos, 2);
    })
    TEST("verifies no open bracket before closing bracket",{
        Token tokens[] = ARRAY(ON, GRP_CLOSE, ON, GRP_OPEN);
        SyntaxVerification result = verify_syntax(tokens,4);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_NO_GROUP_TO_CLOSE);
    })
    TEST("verifies no unclosed bracket",{
        Token tokens[] = ARRAY(ON, GRP_OPEN, GRP_OPEN, ON, GRP_CLOSE);
        SyntaxVerification result = verify_syntax(tokens,5);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_UNCLOSED_GROUP);
    })
    TEST("verifies no error",{
        Token tokens[] = ARRAY(ON, AND ,GRP_OPEN, GRP_OPEN, ON, GRP_CLOSE, GRP_CLOSE);
        SyntaxVerification result = verify_syntax(tokens,7);
        ASSERT_EQUALS(result.has_error, 0);
    })
    TEST("verifies no empty bracket",{
        Token tokens[] = ARRAY(GRP_OPEN, GRP_CLOSE);
        SyntaxVerification result = verify_syntax(tokens,2);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_VALUE_MISSING);
    })
    TEST("verifies no value followed by value",{
        Token tokens[] = ARRAY(ON, OFF);
        SyntaxVerification result = verify_syntax(tokens,2);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_COMBINATOR_MISSING);
    })
    TEST("verifies no value followed by value even with combinator",{
        Token tokens[] = ARRAY(ON, NOT ,OFF);
        SyntaxVerification result = verify_syntax(tokens,3);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_COMBINATOR_MISSING);
    })
    TEST("verifies no combinator followed by combinator",{
        Token tokens[] = ARRAY(AND, OR);
        SyntaxVerification result = verify_syntax(tokens,2);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_VALUE_MISSING);
    })
    TEST("verifies not to many elements in group",{
        Token tokens[] = ARRAY(GRP_OPEN, ON, AND, OFF, OR, ON, GRP_CLOSE);
        SyntaxVerification result = verify_syntax(tokens,7);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP);
    })
    TEST("verifies not to many elements top level",{
        Token tokens[] = ARRAY(ON, AND, OFF, OR, ON);
        SyntaxVerification result = verify_syntax(tokens,5);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP);
    })
    TEST("cuts group slice to size (cuts off end after group close and the brackets)", {
        Token tokens[] = ARRAY(GRP_OPEN, ON, GRP_CLOSE, ON);
        Slice slice = new_slice(tokens, 0, 3);
        Slice grou_slice = cut_group_slice_to_size(slice);

        ASSERT_EQUALS(grou_slice.start, 1);
        ASSERT_EQUALS(grou_slice.end, 1);
    })
    TEST("effective not count", {
        Token even_nots_1[] = ARRAY(NOT, NOT, NOT, NOT, ON);
        Slice en1_slice = new_slice(even_nots_1, 0, 4);
        int en1_result = effective_not_count(en1_slice);
        ASSERT_EQUALS(en1_result, 0);

        Token even_nots_2[] = ARRAY(ON);
        Slice en2_slice = new_slice(even_nots_2, 0, 0);
        int en2_result = effective_not_count(en2_slice);
        ASSERT_EQUALS(en2_result, 0);

        Token odd_nots_1[] = ARRAY(NOT, ON);
        Slice en3_slice = new_slice(odd_nots_1, 0, 1);
        int en3_result = effective_not_count(en3_slice);
        ASSERT_EQUALS(en3_result, 1);

        Token odd_nots_2[] = ARRAY(NOT, NOT, NOT, NOT, NOT, ON);
        Slice en4_slice = new_slice(odd_nots_2, 0, 5);
        int en4_result = effective_not_count(en4_slice);
        ASSERT_EQUALS(en4_result, 1);
    })
    TEST("creates leaf for even number of nots", {
        Token tokens[] = ARRAY(NOT, NOT, ON) ;
        Slice slice = new_slice(tokens, 0, 2);
        Node * node =  to_leaf_or_spout_with_leaf(slice);
        ASSERT_EQUALS(node->type, LEAF);
        ASSERT_EQUALS(node->value.value, VALUE_ON);
    })
    TEST("creates leaf for no modifier", {
        Token tokens[] = ARRAY(ON) ;
        Slice slice = new_slice(tokens, 0, 0);
        Node * node =  to_leaf_or_spout_with_leaf(slice);
        ASSERT_EQUALS(node->type, LEAF);
        ASSERT_EQUALS(node->value.value, VALUE_ON);
    })
    TEST("creates sprout with leaf for odd number of nots", {
        Token tokens[] = ARRAY(NOT, NOT, NOT, ON) ;
        Slice slice = new_slice(tokens, 0, 3);
        Node * node =  to_leaf_or_spout_with_leaf(slice);
        ASSERT_EQUALS(node->type, SPROUT);
        ASSERT_EQUALS(node->tip->type, LEAF);
        ASSERT_EQUALS(node->tip->value.value, VALUE_ON);
    })
    TEST("builds leaf", {
        Token tokens[] = ARRAY(NOT, NOT, ON);
        Node node =  build_tree_2(tokens,0, 2);
        ASSERT_EQUALS(node.type, LEAF);
        ASSERT_EQUALS(node.value.value, VALUE_ON);
    })
    TEST("builds sprout with leaf", {
        Token tokens[] = ARRAY(NOT, ON);
        Node node =  build_tree_2(tokens, 0, 1);
        ASSERT_EQUALS(node.type, SPROUT);
        ASSERT_EQUALS(node.value.modifier, MODIFIER_NOT);
        ASSERT_EQUALS(node.tip->type, LEAF);
        ASSERT_EQUALS(node.tip->value.value, VALUE_ON);
    })
    TEST("builds fork", {
        Token tokens[] = ARRAY(ON, OR, OFF);
        Node node =  build_tree_2(tokens, 0, 2);
        ASSERT_EQUALS(node.type, FORK);
        ASSERT_EQUALS(node.value.combinator, COMBINATOR_OR);
        ASSERT_EQUALS(node.left->type, LEAF);
        ASSERT_EQUALS(node.left->value.value, VALUE_ON);
        ASSERT_EQUALS(node.right->type, LEAF);
        ASSERT_EQUALS(node.right->value.value, VALUE_OFF);
    })
    TEST("builds fork with group brackets", {
        Token tokens[] = ARRAY(GRP_OPEN, ON, OR, OFF, GRP_CLOSE);
        Node node =  build_tree_2(tokens, 0, 4);
        ASSERT_EQUALS(node.type, FORK);
        ASSERT_EQUALS(node.value.combinator, COMBINATOR_OR);
        ASSERT_EQUALS(node.left->type, LEAF);
        ASSERT_EQUALS(node.left->value.value, VALUE_ON);
        ASSERT_EQUALS(node.right->type, LEAF);
        ASSERT_EQUALS(node.right->value.value, VALUE_OFF);
    })
})




















