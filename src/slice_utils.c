#include "slice.h"
#include "err.h"
#include "token.h"

// starting on assignment
Slice find_assignment_body(Slice slice){
    if(slice.arr[slice.start].type != SET){
        err("while cutting slice to size. im not at a statement start");
    }
    // in the sence: "brackets a statement" i.e. 'set' and 'in' are the brackets of an assignment
    int bracket_counter = 0;

    Token token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        token = slice.arr[i];
        if(token.type == SET){
            bracket_counter += 1;
        }
        if(token.type == STATEMENT_END){
            bracket_counter -= 1;
        }
        if(bracket_counter == 0){
            break;
        }
    }
    int statement_end_pos = i;

    if(bracket_counter != 0){
        err("while searching for matching 'in'. not found within slice");
    }

    // set identifier = body in -> i.e. start + 3 is start of body
    Slice assignment_slice = { slice.arr, slice.start+3, statement_end_pos-1};
    return assignment_slice;
}
