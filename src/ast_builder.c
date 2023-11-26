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

// starting on group open
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

// starting on assignment
Slice cut_assignment_slice_to_body(Slice slice){
    if(slice.arr[slice.start].type != SET){
        err("while cutting slice to size. im not at a statement start");
    }
    // in the sence: "brackets a statement" i.e. set and ; are the brackets of an assignment
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
        err("while searching for matching ';'. not found within slice");
    }

    // set identifier = body ; -> i.e. start + 3 is start of body
    Slice assignment_slice = { slice.arr, slice.start+3, statement_end_pos-1};
    return assignment_slice;
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

Node * to_leaf_or_spout_with_leaf(Slice slice, NamedObject ** parent_named_objects, int parent_named_objects_count){
    int not_count = effective_not_count(slice);

    Token value_or_identifier = last_token(slice);
    Node * node = malloc(sizeof(Node));
    Node temp_leaf;
    if(is_value(value_or_identifier)){
        temp_leaf = new_leaf(to_value(value_or_identifier), parent_named_objects, parent_named_objects_count);
    }
    else if(is_identifier(value_or_identifier)){
        temp_leaf = new_object_leaf(value_or_identifier.name, parent_named_objects, parent_named_objects_count);
    }
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


Node * build_node(Slice slice, NamedObject ** parent_named_objects, int parent_named_objects_count);

Node * create_node_after_open_bracket_found(
    Slice slice,
    int bracket_open_index,
    NamedObject ** named_objects,
    int named_objects_count
){
    Slice group_slice = cut_group_slice_to_size(new_slice(slice.arr, bracket_open_index, slice.end));

    Node * left_node = build_node(group_slice, named_objects, named_objects_count);
    if(slice.end-1 == group_slice.end){
        return left_node;
    }
    Combinator combinator = to_combinator(token_at(slice, group_slice.end + 2)); // next next after group slice must be the combinator

    Slice right_slice = new_slice(slice.arr, group_slice.end + 3, slice.end); // next next next after group slice is start of right node
    Node * right_node = build_node(right_slice, named_objects, named_objects_count);
    Node temp_fork = new_fork(left_node, right_node, combinator);

    Node * fork = malloc(sizeof(Node));
    memcpy(fork, &temp_fork, sizeof(Node));



    int not_count;
    if(bracket_open_index == slice.start){
        not_count = 0;
    }else {
        Slice left_from_group = new_slice(slice.arr, slice.start, bracket_open_index - 1);
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

Node * create_node_after_value_found(
    Slice slice,
    int value_index,
    NamedObject ** parent_named_objects,
    int parent_named_objects_count
){

    Slice left_slice = new_slice(slice.arr, slice.start, value_index);
    Node * left_node = to_leaf_or_spout_with_leaf(left_slice, parent_named_objects, parent_named_objects_count);
    if(value_index == slice.end){
        return left_node;
    }

    Combinator combinator = to_combinator(token_at(slice, value_index+1)); // next token must be combinator

    Slice right_slice = new_slice(slice.arr, value_index+2, slice.end); // next next token must is start of right node
    Node * right_node = build_node(right_slice, parent_named_objects, parent_named_objects_count);
    Node temp_fork = new_fork(left_node, right_node, combinator);
    Node * fork = malloc(sizeof(Node));
    memcpy(fork, &temp_fork, sizeof(Node));
    return fork;
}


Node * build_node(Slice slice, NamedObject ** parent_named_objects, int parent_named_objects_count){

    NamedObject ** named_objects = malloc(sizeof(NamedObject*) * parent_named_objects_count);
    memcpy(named_objects, parent_named_objects, sizeof(NamedObject*) * parent_named_objects_count);
    int named_objects_count = parent_named_objects_count;

    int success = 0;

    // algorithm:
    // searching for either a value or a open bracket
    // if the first thing found is a value (or identifier)
    //  -> left side is a leaf/sprout with leaf
    //  -> after value must be combinator
    //  -> after combinator build node from the right side
    // if the first thing found is a open bracket
    //  -> left side can be a fork, build node from that
    //  -> must search for close and then the next thing must be a combinator
    //  -> after combinator build node from right side

    Token token;
    int significant_token_index = -1;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        token = slice.arr[i];

        if(token.type == SET){
            int identifier_pos = i+1;
            Slice body_slice = cut_assignment_slice_to_body(new_slice(slice.arr, i, slice.end));
            Node * ref = build_node(body_slice, named_objects, named_objects_count);

            NamedObject * temp_named_object = malloc(sizeof(NamedObject));
            NamedObject constant = new_constant(slice.arr[identifier_pos].name, ref);
            memcpy(temp_named_object, &constant , sizeof(NamedObject));

            named_objects_count += 1;
            NamedObject ** temp_named_objects = realloc(named_objects, sizeof(NamedObject*) * (named_objects_count));
            if(temp_named_objects == NULL){
                err("failed to allocate space");
            }
            named_objects = temp_named_objects;
            named_objects[named_objects_count-1] = temp_named_object;

            i = body_slice.end + 1; // jump to statement end token
        }

        if(is_grp_open(token) || is_value(token) || is_identifier(token)) {
            significant_token_index = i;
            break;
        }
    };

    if(significant_token_index == -1){
        err("while building tree");
    }
    Node * node = malloc(sizeof(Node));
    if(is_value(token) || is_identifier(token)){
        int value_index = significant_token_index;
        return create_node_after_value_found(slice, value_index, named_objects, named_objects_count);
    }else{
        // else must be bracket
        int bracket_open_pos = significant_token_index;
        return create_node_after_open_bracket_found(slice, bracket_open_pos, named_objects, named_objects_count);
        }
}

Node build_tree(Token * tokens, size_t start_token, size_t end_token){
    Slice slice = new_slice(tokens, start_token, end_token);

    NamedObject ** initial_named_objects = NULL;
    int initial_named_objects_count = 0;

    return *build_node(slice, initial_named_objects, initial_named_objects_count);
}
