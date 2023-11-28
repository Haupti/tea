#include "sanity_check.h"
#include "slice_utils.h"
#include "token.h"
#include "slice.h"
#include <string.h>
#include <stdio.h>

SanityCheck sanity_check_with_error(SanityErrorType type, Token at_token, int at_position){
    SanityError sanity_error = { at_token, at_position, type };
    union SanityCheckResult result;
    result.error = sanity_error;
    SanityCheck sanity_check = { 1, result };
    return sanity_check;
}

SanityCheck sanity_check_without_error(){
    union SanityCheckResult result;
    result.null = '0';
    SanityCheck sanity_check = { 0, result };
    return sanity_check;
}

typedef struct Assignment {
   Token identifier;
   Slice body;
} Assignment;

// returns 0 if not found, 1 for use before assignment and 2 for overwrite
int has_use_or_overwrite_of_identifier(Slice slice, Token identifier){
    Token token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        token = slice.arr[i];
        if(token.type == SET){
            Token next_token = slice.arr[i+1];
            if(strcmp(next_token.name, identifier.name) == 0){
                return 2;
            }
        }
        if(token.type == IDENTIFIER && strcmp(token.name, identifier.name) == 0){
            return 1;
        }
    }
    return 0;
}

SanityCheck perform_sanity_check(Slice slice){
    Token token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        token = slice.arr[i];
        if(token.type == SET){
            Token next_token = slice.arr[i+1];
            Slice assignment_body = find_assignment_body(new_slice(slice.arr,i,slice.end));
            int is_contains_identifier = has_use_or_overwrite_of_identifier(assignment_body,next_token);
            if(is_contains_identifier == 1){
                return sanity_check_with_error(SNITY_ERR_USE_BEFORE_ASSIGNMENT, next_token, i+1);
            }
            else if(is_contains_identifier == 2){
                return sanity_check_with_error(SNITY_ERR_NO_OVERWRITE, next_token, i+1);
            }
        }
    }
    return sanity_check_without_error();
}

SanityCheck sanity_check(Token * tokens, size_t tokens_len){
    Slice slice = new_slice(tokens, 0, tokens_len-1);
    return perform_sanity_check(slice);
}

void print_snty_err_and_exit(SanityCheck snty_check){
    if(snty_check.has_error){
        switch(snty_check.result.error.error_type){
            case SNITY_ERR_USE_BEFORE_ASSIGNMENT:{
                    printf("SANITY ERROR: identifiers cannot be referenced before assignment\n");
                    break;
                }
            case SNITY_ERR_NO_OVERWRITE:{
                    printf("SANITY ERROR: no overwrite of identifier\n");
                    break;
            }
        }
        printf("SANITY ERROR: at token position %d\n", snty_check.result.error.at_position);
        exit(EXIT_FAILURE);
    }
}
