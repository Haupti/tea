#include <string.h>
#include "assert.h"
#include "assert_helpers.h"
#include <stdio.h>
#include "../src/token.h"
#include "../src/lexer.h"

MODULAR_DESCRIBE(read_tokens_tests,{
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
        ASSERT_INT_EQUALS(count,1);
        char show[20];
        show_token(show, &tokens[0]);
        ASSERT_STR_EQUALS(show,"ON");
    })
    TEST("reads value definition",{
        Token tokens[10];
        int count = read_tokens(tokens, "set a = 1 in a", 15);
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
        char input[] = "set b = (set a = 1 in !1 | 1) in b";
        int count = read_tokens(tokens, input , strlen(input)+1);
        char show[25];
        char expected[16][25] = ARRAY("SET", "IDENTIFIER(b)", "ASSIGNMENT_OPERATOR", "GRP_OPEN", "SET", "IDENTIFIER(a)", "ASSIGNMENT_OPERATOR", "ON", "STATEMENT_END", "NOT", "ON", "OR", "ON", "GRP_CLOSE", "STATEMENT_END", "IDENTIFIER(b)");
        ASSERT_INT_EQUALS(count, 16);
        for(int i = 0; i < count; i++){
            show_token(show, &tokens[i]) ;
            ASSERT_STR_EQUALS(show ,expected[i]);
        }
    })
    TEST("reads conditional",{
        Token tokens[20];
        char input[] = "set a = 1 in if a then 0 else 1 end";
        int count = read_tokens(tokens, input , strlen(input)+1);
        char show[25];
        char expected[12][25] = ARRAY("SET", "IDENTIFIER(a)", "ASSIGNMENT_OPERATOR", "ON", "STATEMENT_END", "IF", "IDENTIFIER(a)", "THEN", "OFF", "ELSE", "ON", "END");
        ASSERT_INT_EQUALS(count, 12);
        for(int i = 0; i < count; i++){
            show_token(show, &tokens[i]) ;
            ASSERT_STR_EQUALS(show ,expected[i]);
        }
    })
    TEST("reads conditional in complex setup",{
        Token tokens[35];
        char input[] = "set a = 1 | 0 in\nset b = (a & 1) in\nset c = b in\n(if a then b else c end | 0)\n";
        int count = read_tokens(tokens, input , strlen(input)+1);
        char show[25];
        char expected[32][25] = ARRAY("SET", "IDENTIFIER(a)", "ASSIGNMENT_OPERATOR", "ON", "OR", "OFF", "STATEMENT_END",
                                      "SET", "IDENTIFIER(b)", "ASSIGNMENT_OPERATOR", "GRP_OPEN", "IDENTIFIER(a)", "AND", "ON", "GRP_CLOSE", "STATEMENT_END",
                                      "SET", "IDENTIFIER(c)", "ASSIGNMENT_OPERATOR", "IDENTIFIER(b)", "STATEMENT_END",
                                      "GRP_OPEN", "IF", "IDENTIFIER(a)", "THEN", "IDENTIFIER(b)", "ELSE", "IDENTIFIER(c)", "END",  "OR", "OFF", "GRP_CLOSE");
        ASSERT_INT_EQUALS(count, 32);
        for(int i = 0; i < count; i++){
            show_token(show, &tokens[i]) ;
            ASSERT_STR_EQUALS(show ,expected[i]);
        }
    })
    TEST("reads function definition",{
        Token tokens[35];
        char input[] = "define funcname a b as a | b done\n";
        int count = read_tokens(tokens, input , strlen(input)+1);
        char show[25];
        char expected[9][25] = ARRAY("DEFINE", "IDENTIFIER(funcname)", "IDENTIFIER(a)", "IDENTIFIER(b)", "DEFINE_AS", "IDENTIFIER(a)", "OR", "IDENTIFIER(b)", "DONE");
        ASSERT_INT_EQUALS(count, 9);
        for(int i = 0; i < count; i++){
            show_token(show, &tokens[i]) ;
            ASSERT_STR_EQUALS(show ,expected[i]);
        }
    })
    TEST("reads function definition and usage",{
        Token tokens[35];
        char input[] = "define funcname a b as a | b done\nfuncname 1 0\n";
        int count = read_tokens(tokens, input , strlen(input)+1);
        char show[25];
        char expected[12][25] = ARRAY("DEFINE", "IDENTIFIER(funcname)", "IDENTIFIER(a)", "IDENTIFIER(b)", "DEFINE_AS", "IDENTIFIER(a)", "OR", "IDENTIFIER(b)", "DONE", "IDENTIFIER(funcname)", "ON", "OFF");
        ASSERT_INT_EQUALS(count, 12);
        for(int i = 0; i < count; i++){
            show_token(show, &tokens[i]) ;
            ASSERT_STR_EQUALS(show ,expected[i]);
        }
    })
})
