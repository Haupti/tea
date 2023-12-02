#include "slice.h"
#include "token.h"
#include "err.h"
#include <stdio.h>

Slice find_bracket_slice(Slice slice) {
    TokenType br_open_t = slice.arr[slice.start].type;
    TokenType br_close_t;
    switch(br_open_t){
        case GRP_OPEN:
            br_close_t = GRP_CLOSE;
            break;
        case SET:
            br_close_t = STATEMENT_END;
            break;
        case IF:
            br_close_t = THEN;
            break;
        case THEN:
            br_close_t = ELSE;
            break;
        case ELSE:
            br_close_t = END;
            break;
        default:
            err("cant seach for bracket, this is not the start of one");
    }

    int br_count = 0;
    int success = 0;
    Token token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        token = slice.arr[i];
        if(token.type == br_open_t){
            br_count += 1;
        }
        if(token.type == br_close_t){
            br_count -= 1;
        }
        if(br_count == 0){
            success = 1;
            break;
        }
    }
    if(!success){
        printf("failed searching for bracket %s\n", TOKEN_TYPE_STR(br_open_t));
        exit(EXIT_FAILURE);
    }
    Slice body_slice = {slice.arr, slice.start+1, i-1};
    return body_slice;
}
