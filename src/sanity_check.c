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

// returns 1 if identifier is used, 2 if overwritten/shadowed and 0 if everything is fine
int has_use_of_identifier(Slice slice, Token identifier){
    Token token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        token = slice.arr[i];
        if(token.type == IDENTIFIER && strcmp(token.name, identifier.name) == 0){
            return 1;
        }
        if(token.type == SET){
            Token next_token = slice.arr[i+1];
            if(strcmp(next_token.name, identifier.name) == 0){
                return 2;
            }
        }

    }
    return 0;
}

typedef struct IdentifierIndexPair {
    char * identifier_name;
    int index;
} IdentifierIndexPair;
// set a = (a | a) | a;

SanityCheck perform_sanity_check(Slice slice){
    // i dont want to deal with malloc here
    int max_assignments = ((slice.end - slice.start + 1) / 5 ) + 5;
    int max_uses = ((slice.end - slice.start + 1) / 3 ) + 5; // estimation, might be too high
    IdentifierIndexPair assigned_identifies[max_assignments];
    IdentifierIndexPair uses_identifiers[max_uses];
    int assigned_identifies_index = 0;
    int uses_identifiers_index = 0;

    // scan for assignments

    Token token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        token = slice.arr[i];
        if(token.type == SET){
            Token next_token = slice.arr[i+1];

            // problems within the assignment
            Slice assignment_body = find_assignment_body(new_slice(slice.arr,i,slice.end));
            int is_contains_identifier = has_use_of_identifier(assignment_body,next_token);
            if(is_contains_identifier == 1){
                return sanity_check_with_error(SNITY_ERR_USE_BEFORE_ASSIGNMENT, next_token, i+1);
            }
            if(is_contains_identifier == 2){
                return sanity_check_with_error(SNITY_ERR_NO_OVERWRITE, next_token, i+1);
            }

            // for later checks of overwrite etc
            IdentifierIndexPair pair = {next_token.name, i+1};
            assigned_identifies[assigned_identifies_index] = pair;
            assigned_identifies_index += 1;
        }
        if(token.type == IDENTIFIER && ((i != slice.start && slice.arr[i-1].type != SET) || i == slice.start)){
            IdentifierIndexPair pair = {token.name, i};
            uses_identifiers[uses_identifiers_index] = pair;
            uses_identifiers_index += 1;
        }

    }
    // search for duplicates
    int j;
    int k;
    for(j = 0;  j < assigned_identifies_index; j++){
        for(k = 0;  k < assigned_identifies_index; k++){
            if(j == k) continue;
            if(strcmp(assigned_identifies[j].identifier_name, assigned_identifies[k].identifier_name) == 0){
                IdentifierIndexPair found_dup = assigned_identifies[k];
                return sanity_check_with_error(SNITY_ERR_NO_OVERWRITE, slice.arr[found_dup.index], found_dup.index);
            }
        }
    }

    int l;
    int m;
    IdentifierIndexPair assigned;
    IdentifierIndexPair use;
    for(l = 0; l < uses_identifiers_index; l++){
        use = uses_identifiers[l];
        for(m = 0; m < assigned_identifies_index; m++){
            assigned = assigned_identifies[m];
            if(strcmp(assigned.identifier_name, use.identifier_name) == 0 && assigned.index > use.index){
                return sanity_check_with_error(SNITY_ERR_USE_BEFORE_ASSIGNMENT, slice.arr[l], l);
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
