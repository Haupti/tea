#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "ast_node.h"
#include "cursor.h"
#include "err.h"
#include "slice.h"



int call_counter(){
    static int calls = 0;
    calls += 1;
    return calls-1;
}


// expects the cursor to be at the position of grp_close
// returns value between start and end: the position of the matching grp_close
// error if no matching grp_close could be found
int find_matching_grp_close(Cursor * original_cursor){
    call_counter();
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    if(current(cursor).type != GRP_OPEN){
        err("while searching bracket: cannot find matching ')' if im not at a '('");
    }
    Cursor temp = copy_cursor(cursor);
    Token token = current(&temp);
    int bracket_counter = 1;
    while(has_next(&temp)){
        token = next(&temp);
        switch(token.type){
            case GRP_OPEN:
                bracket_counter += 1;
                break;
            case GRP_CLOSE:
                bracket_counter -= 1;
                if(bracket_counter == 0){
                    return temp.position;
                }
                break;
            default:
                break;
        }
    }
    err("while searching bracket: no matching ')' found");
    return -1;
}



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

Cursor left_expression_cursor(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;
    const int start = cursor->position;
    int end;

    while(1){
        if(is_grp_open(current(cursor))){
            end = find_matching_grp_close(cursor);
            break;
        }
        if(is_value(current(cursor))){
            end = cursor->position;
            break;
        }
        next(cursor);
    }

    Cursor new_cursor = {cursor->arr, start, end, start};
    return new_cursor;
}


int find_matching_combinator(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    while(1){ // must find an combinator
        if(is_grp_open(current(cursor))){
            int matching_close = find_matching_grp_close(cursor);
            jump_to(cursor, matching_close);
            next(cursor);
            break;
        }
        if(is_value(current(cursor))){
            next(cursor);
            break;
        }
        next(cursor);
    }
    return cursor->position;
}


Combinator find_combinator(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    int combinator_pos = find_matching_combinator(cursor) ;
    Combinator combinator = to_combinator(cursor->arr[combinator_pos]);

    return combinator;
}

Cursor right_expression_cursor(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    int combinator_pos = find_matching_combinator(cursor);
    jump_to(cursor, combinator_pos);
    next(cursor); // at start right expression
    const int start = cursor->position;

    int end;
    while(1){ // must find an expression end
        if(is_grp_open(current(cursor))){
            end = find_matching_grp_close(cursor);
            break;
        }
        if(is_value(current(cursor))){
            end = cursor->position;
            break;
        }
        next(cursor);
    }

    Cursor new_cursor = {cursor->arr, start, end, start};
    return new_cursor;
}

Cursor create_cursor_for_inner_expession(Cursor * cursor){
   Cursor inner_cursor = new_cursor(cursor->arr, (cursor->start + 1), (cursor->end -1));
   return inner_cursor;
}

Cursor create_cursor_without_first(Cursor * cursor){
   Cursor inner_cursor = new_cursor(cursor->arr, (cursor->start + 1), cursor->end);
   return inner_cursor;
}

int holds_a_sprout(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    Token token = current(cursor);
    if(token.type == GRP_OPEN && find_matching_grp_close(cursor) == cursor->end){
        return 1;
    }
    else if(is_value(token)){
        return cursor->position == cursor->end;
    }
    else { // modifier or something else
        Cursor shrinked_cursor = create_cursor_without_first(cursor);
        return holds_a_sprout(&shrinked_cursor);
    }
}

Node * build_node(Cursor cursor){
    if(size(&cursor) == 1){
        Node * leaf = malloc(sizeof(Node));
        Node temp_leaf = new_leaf(to_value(current(&cursor)));
        memcpy(leaf, &temp_leaf, sizeof(Node));
        return leaf;
    }
    else if(holds_a_sprout(&cursor)){
        Modifier modifier;
        Cursor inner_cursor;
        if(current(&cursor).type == NOT){
            modifier = MODIFIER_NOT;
            inner_cursor = create_cursor_without_first(&cursor);
        }
        else { // must be a bracket then
            modifier = MODIFIER_ID;
            inner_cursor = create_cursor_for_inner_expession(&cursor);
        }
        Node * sprout = malloc(sizeof(Node));
        build_node(inner_cursor);
        Node temp_sprout = new_sprout(build_node(inner_cursor), modifier);
        memcpy(sprout, &temp_sprout, sizeof(Node));
        return sprout;
    }
    else {
        Node * left = build_node(left_expression_cursor(&cursor));
        Node * right = build_node(right_expression_cursor(&cursor));
        Combinator combinator = find_combinator(&cursor);
        NodeValue node_value;
        node_value.combinator = combinator;
        Node node = {FORK, node_value, left, right};
        Node * node_ptr = malloc(sizeof(Node));
        memcpy(node_ptr, &node, sizeof(Node));
        return node_ptr;
    }
}

Node build_tree(Token * tokens, size_t start_token, size_t end_token){
    Cursor cursor = new_cursor(tokens, start_token, end_token);
    return *build_node(cursor);
}

// V2 #######################################################

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

Node build_tree_2(Token * tokens, size_t start_token, size_t end_token){
    Slice slice = new_slice(tokens, start_token, end_token);
    return *build_node_2(slice);
}





















