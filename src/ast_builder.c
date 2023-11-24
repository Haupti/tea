#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "ast_node.h"
#include "err.h"
#include "slice.h"

Value to_value(Token token){
    Value value;
    switch(token.type){
        case OFF:
            value = VALUE_OFF;
            break;
        case ON:
            value = VALUE_ON;
            break;
        default:
            err("while parsing: token is not a value");
            break;
    }
    return value;
}

Combinator to_combinator(Token token){
    Combinator combinator;
    switch(token.type){
        case AND:
            combinator = COMBINATOR_AND;
            break;
        case OR:
            combinator = COMBINATOR_OR;
            break;
        default:
            err("while parsing: token is not a combinator");
            break;
    }
    return combinator;
}

Slice cut_group_slice_to_size(Slice slice){
    if(slice.arr[slice.start].type != GRP_OPEN){
        err("while cutting slice to size. im not at a group start");
    }
    int bracket_counter = 0;

    Token token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        token = slice.arr[i];
        if(token.type == GRP_OPEN){
            bracket_counter += 1;
        }
        if(token.type == GRP_CLOSE){
            bracket_counter -= 1;
        }
        if(bracket_counter == 0){
            break;
        }
    }
    int grp_close_pos = i;

    if(bracket_counter != 0){
        err("while searching for matching ')'. not found within slice");
    }

    Slice group_slice = { slice.arr, slice.start+1, grp_close_pos-1};
    return group_slice;
}

int effective_not_count(Slice slice){
    if(slice.arr == NULL){
        err("while optimizing not's: no tokens");
    }
    int i;
    int not_count = 0;
    Token token;
    for(i = slice.start; i <= slice.end; i++){
        token = token_at(slice, i);
        if(token.type == NOT){
            not_count += 1;
        }
    }
    return not_count % 2;
}

Node * to_leaf_or_spout_with_leaf(Slice slice){
    int not_count = effective_not_count(slice);

    Node * node = malloc(sizeof(Node));
    Node temp_leaf = new_leaf(to_value(last_token(slice)));
    memcpy(node, &temp_leaf, sizeof(Node));
    if(not_count == 0){
        return node;
    }
    else{
        Node * sprout = malloc(sizeof(Node));
        Node temp_sprout = new_sprout(node, MODIFIER_NOT);
        memcpy(sprout, &temp_sprout, sizeof(Node));
        return sprout;
    }
}

Node * build_node_2(Slice slice){

    int success = 0;

    Token token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        token = slice.arr[i];

        if(is_grp_open(token) || is_value(token)) {
            success = 1;
            break;
        }
    };

    if(!success){
        err("while building tree");
    }

    Node * node = malloc(sizeof(Node));
    if(is_value(token)){
        int value_index = i;
        Slice left_slice = new_slice(slice.arr, slice.start, value_index);
        Node * left_node = to_leaf_or_spout_with_leaf(left_slice);
        if(value_index == slice.end){
            return left_node;
        }

        Combinator combinator = to_combinator(token_at(slice, value_index+1)); // next token must be combinator

        Slice right_slice = new_slice(slice.arr, value_index+2, slice.end); // next next token must is start of right node
        Node * right_node = build_node_2(right_slice);
        Node temp_fork = new_fork(left_node, right_node, combinator);
        Node * fork = malloc(sizeof(Node));
        memcpy(fork, &temp_fork, sizeof(Node));
        return fork;
    }
    // else must be bracket
    int bracket_open_pos = i;


    Slice group_slice = cut_group_slice_to_size(new_slice(slice.arr, bracket_open_pos, slice.end));

    Node * left_node = build_node_2(group_slice);
    if(slice.end-1 == group_slice.end){
        return left_node;
    }
    Combinator combinator = to_combinator(token_at(slice, group_slice.end + 2)); // next next after group slice must be the combinator

    Slice right_slice = new_slice(slice.arr, group_slice.end + 3, slice.end); // next next next after group slice is start of right node
    Node * right_node = build_node_2(right_slice);
    Node temp_fork = new_fork(left_node, right_node, combinator);

    Node * fork = malloc(sizeof(Node));
    memcpy(fork, &temp_fork, sizeof(Node));



    int not_count;
    if(bracket_open_pos == slice.start){
        not_count = 0;
    }else {
        Slice left_from_group = new_slice(slice.arr, slice.start, bracket_open_pos - 1);
        not_count = effective_not_count(left_from_group);
    }
    if(not_count == 0){
        return fork;
    }
    else {
        Node * sprout = malloc(sizeof(Node));
        Node temp_sprout = new_sprout(fork, MODIFIER_NOT);
        memcpy(sprout, &temp_sprout, sizeof(Node));
        return sprout;
    }
}

Node build_tree(Token * tokens, size_t start_token, size_t end_token){
    Slice slice = new_slice(tokens, start_token, end_token);
    return *build_node_2(slice);
}

/*
(0) search frist bracket, combinator or value

(1A) found bracket first
    (2) goto end of that bracket
    (3) this is 1. cursor -> (0) with this cursor
    (4) check if cursor ends at bracket
        (4.a) if not search combinator -> everything right of the combinator is the 2. cursor -> (0) with this cursor
        (4.b) if so -> (5)
    (5) collect my modifiers and reduce (!! -> no modifier, !!! -> not, ...)
    (6a) if i have none, return 1. node
    (6b) if i have one, i am a sprout with 1. node as tip

(1B) found value
    (2) everything left of the is a leaf -> collect my modifiers and evaluate to a value, this is 1. node
    (3) the combinator is my node value
    (3) everything right of the combinator is the 2. node -> (0) with that node

*/





















