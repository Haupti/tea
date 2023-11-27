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

Node new_top_level_leaf(Value value){
    return new_leaf(value, NULL, 0);
}

DESCRIBE("read_tokens", {
    TEST("reads 1 & 0",{
        Token tokens[10];
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
        Token tokens[10];
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
    TEST("ignores comment until newline",{
        Token tokens[10];
        int count = read_tokens(tokens, "-- comment\n 1", 13);
        ASSERT_EQUALS(count,1);
        char show[20];
        show_token(show, &tokens[0]);
        ASSERT_STR_EQUALS(show,"ON");
    })
    TEST("reads value definition",{
        Token tokens[10];
        int count = read_tokens(tokens, "set a = 1; a", 13);
        ASSERT_EQUALS(count,6);
        char show[20];
        show_token(show, &tokens[0]);
        ASSERT_STR_EQUALS(show,"SET");
        show_token(show, &tokens[1]);
        ASSERT_STR_EQUALS(show,"IDENTIFIER(a)");
        show_token(show, &tokens[2]);
        ASSERT_STR_EQUALS(show,"ASSIGNMENT_OPERATOR");
        show_token(show, &tokens[3]);
        ASSERT_STR_EQUALS(show,"ON");
        show_token(show, &tokens[4]);
        ASSERT_STR_EQUALS(show,"STATEMENT_END");
        show_token(show, &tokens[5]);
        ASSERT_STR_EQUALS(show,"IDENTIFIER(a)");
    })
    TEST("reads value definition with additional stuff",{
        Token tokens[20];
        char input[] = "set b = (set a = 1; !1 | 1); b";
        int count = read_tokens(tokens, input , strlen(input));
        char show[25];
        char expected[16][25] = ARRAY("SET", "IDENTIFIER(b)", "ASSIGNMENT_OPERATOR", "GRP_OPEN", "SET", "IDENTIFIER(a)", "ASSIGNMENT_OPERATOR", "ON", "STATEMENT_END", "NOT", "ON", "OR", "ON", "GRP_CLOSE", "STATEMENT_END", "IDENTIFIER(b)");
        ASSERT_EQUALS(count, 16);
        for(int i = 0; i < count; i++){
            show_token(show, &tokens[i]) ;
            ASSERT_STR_EQUALS(show ,expected[i]);
        }
    })
    TEST("evaluates leaf",{
        Node node = new_top_level_leaf(VALUE_ON);
        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("evaluates flat fork",{
        Node left = new_top_level_leaf(VALUE_ON);
        Node right = new_top_level_leaf(VALUE_OFF);
        Node node = new_fork(&left, &right, COMBINATOR_AND);
        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_OFF);
    })
    TEST("evaluates nested fork (left is leaf)",{
        Node left = new_top_level_leaf(VALUE_ON);

        Node left_d1 = new_top_level_leaf(VALUE_OFF);
        Node right_d1 = new_top_level_leaf(VALUE_ON);

        Node node_d1 = new_fork(&left_d1, &right_d1, COMBINATOR_OR);

        Node node = new_fork(&left, &node_d1, COMBINATOR_AND);

        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("evaluates nested fork (right is leaf)",{
        Node right = new_top_level_leaf(VALUE_ON);

        Node left_d1 = new_top_level_leaf(VALUE_ON);
        Node right_d1 = new_top_level_leaf(VALUE_ON);

        Node node_d1 = new_fork(&left_d1, &right_d1, COMBINATOR_AND);

        Node node = new_fork(&node_d1, &right, COMBINATOR_AND);

        Value value = evaluate_node(&node);
        ASSERT_EQUALS(value, VALUE_ON);
    })
    TEST("verifies no open bracket before closing bracket",{
        Token tokens[] = ARRAY(new_token(ON), new_token(GRP_CLOSE), new_token(ON), new_token(GRP_OPEN));
        SyntaxVerification result = verify_syntax(tokens,4);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_NO_GROUP_TO_CLOSE);
    })
    TEST("verifies no unclosed bracket",{
        Token tokens[] = ARRAY(new_token(ON), new_token(GRP_OPEN), new_token(GRP_OPEN), new_token(ON), new_token(GRP_CLOSE));
        SyntaxVerification result = verify_syntax(tokens,5);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_UNCLOSED_GROUP);
    })
    TEST("verifies no error",{
        Token tokens[] = ARRAY(new_token(ON), new_token(AND) ,new_token(GRP_OPEN), new_token(GRP_OPEN), new_token(ON), new_token(GRP_CLOSE), new_token(GRP_CLOSE));
        SyntaxVerification result = verify_syntax(tokens,7);
        ASSERT_EQUALS(result.has_error, 0);
    })
    TEST("verifies no empty bracket",{
        Token tokens[] = ARRAY(new_token(GRP_OPEN), new_token(GRP_CLOSE));
        SyntaxVerification result = verify_syntax(tokens,2);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_VALUE_MISSING);
    })
    TEST("verifies no value followed by value",{
        Token tokens[] = ARRAY(new_token(ON), new_token(OFF));
        SyntaxVerification result = verify_syntax(tokens,2);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_COMBINATOR_MISSING);
    })
    TEST("verifies no value followed by value even with combinator",{
        Token tokens[] = ARRAY(new_token(ON), new_token(NOT), new_token(OFF));
        SyntaxVerification result = verify_syntax(tokens,3);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_COMBINATOR_MISSING);
    })
    TEST("verifies no combinator followed by combinator",{
        Token tokens[] = ARRAY(new_token(AND), new_token(OR));
        SyntaxVerification result = verify_syntax(tokens,2);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_VALUE_MISSING);
    })
    TEST("verifies not to many elements in group",{
        Token tokens[] = ARRAY(new_token(GRP_OPEN), new_token(ON), new_token(AND), new_token(OFF), new_token(OR), new_token(ON), new_token(GRP_CLOSE));
        SyntaxVerification result = verify_syntax(tokens,7);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP);
    })
    TEST("verifies not to many elements top level",{
        Token tokens[] = ARRAY(new_token(ON), new_token(AND), new_token(OFF), new_token(OR), new_token(ON));
        SyntaxVerification result = verify_syntax(tokens,5);
        ASSERT_EQUALS(result.has_error, 1);
        ASSERT_EQUALS(result.result.error.error_type, SYNTX_ERR_TOO_MANY_TOKENS_IN_GROUP);
    })
    TEST("cuts group slice to size (cuts off end after group close and the brackets)", {
        Token tokens[] = ARRAY(new_token(GRP_OPEN), new_token(ON), new_token(GRP_CLOSE), new_token(ON));
        Slice slice = new_slice(tokens, 0, 3);
        Slice grou_slice = cut_group_slice_to_size(slice);

        ASSERT_EQUALS(grou_slice.start, 1);
        ASSERT_EQUALS(grou_slice.end, 1);
    })
    TEST("effective not count", {
        Token even_nots_1[] = ARRAY(new_token(NOT), new_token(NOT), new_token(NOT), new_token(NOT), new_token(ON));
        Slice en1_slice = new_slice(even_nots_1, 0, 4);
        int en1_result = effective_not_count(en1_slice);
        ASSERT_EQUALS(en1_result, 0);

        Token even_nots_2[] = ARRAY(new_token(ON));
        Slice en2_slice = new_slice(even_nots_2, 0, 0);
        int en2_result = effective_not_count(en2_slice);
        ASSERT_EQUALS(en2_result, 0);

        Token odd_nots_1[] = ARRAY(new_token(NOT), new_token(ON));
        Slice en3_slice = new_slice(odd_nots_1, 0, 1);
        int en3_result = effective_not_count(en3_slice);
        ASSERT_EQUALS(en3_result, 1);

        Token odd_nots_2[] = ARRAY(new_token(NOT), new_token(NOT), new_token(NOT), new_token(NOT), new_token(NOT), new_token(ON));
        Slice en4_slice = new_slice(odd_nots_2, 0, 5);
        int en4_result = effective_not_count(en4_slice);
        ASSERT_EQUALS(en4_result, 1);
    })
    TEST("creates leaf for even number of nots", {
        Token tokens[] = ARRAY(new_token(NOT), new_token(NOT), new_token(ON));
        Slice slice = new_slice(tokens, 0, 2);
        Node * node =  to_leaf_or_spout_with_leaf(slice);
        ASSERT_EQUALS(node->type, LEAF);
        ASSERT_EQUALS(node->value.value, VALUE_ON);
    })
    TEST("creates leaf for no modifier", {
        Token tokens[] = ARRAY(new_token(ON));
        Slice slice = new_slice(tokens, 0, 0);
        Node * node =  to_leaf_or_spout_with_leaf(slice);
        ASSERT_EQUALS(node->type, LEAF);
        ASSERT_EQUALS(node->value.value, VALUE_ON);
    })
    TEST("creates sprout with leaf for odd number of nots", {
        Token tokens[] = ARRAY(new_token(NOT), new_token(NOT), new_token(NOT), new_token(ON)) ;
        Slice slice = new_slice(tokens, 0, 3);
        Node * node =  to_leaf_or_spout_with_leaf(slice);
        ASSERT_EQUALS(node->type, SPROUT);
        ASSERT_EQUALS(node->tip->type, LEAF);
        ASSERT_EQUALS(node->tip->value.value, VALUE_ON);
    })
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




















