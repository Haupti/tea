#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "ast_node.h"
#include "err.h"
#include "slice.h"
#include "slice_utils.h"

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


// returns the position of the combinator
// or -1 if there is no combinator on the current level
int combinator_position(Slice slice){
    int bracket_counter = 0;
    int combinator_position = -1;
    int i;
    for (i = slice.start; i <= slice.end; i++){
        Token token = slice.arr[i];
        if(is_grp_open(token)){
            bracket_counter += 1;
        } else if(is_grp_close(token)){
            bracket_counter -= 1;
        } else if(is_combinator(token) && bracket_counter == 0){
            combinator_position = i;
            break;
        }
    }
    return combinator_position;
}

int is_allowed_token_after_assignment_end(Token token){
    return is_value(token) || token.type == NOT || token.type == IDENTIFIER || is_grp_open(token);
}

Node * build_node(Slice slice, NamedObject ** parent_named_objects, int parent_named_objects_count);

// expects slice to only contain tokens of the node to create i.e. the assignments must be collected before calling this
Node * create_from_unforked_body(Slice slice, NamedObject ** in_scope_named_objects, int in_scope_named_objects_count){
        Token current_token;
        int nots = 0;
        int current_token_pos;
        for(current_token_pos = slice.start; current_token_pos <= slice.end; current_token_pos++){
            current_token = slice.arr[current_token_pos];
            if(current_token.type == NOT){
                nots = (nots + 1) % 2;
            }
            else if(is_value(current_token)) {
                Node * leaf = create_leaf(to_value(current_token), in_scope_named_objects, in_scope_named_objects_count);
                if(nots != 0){
                    return create_sprout(leaf, MODIFIER_NOT);
                }
                else {
                    return leaf;
                }
                break;
            }
            else if(is_identifier(current_token)){
                Node * leaf = create_object_leaf(current_token.name, in_scope_named_objects, in_scope_named_objects_count);
                if(nots != 0){
                    return create_sprout(leaf, MODIFIER_NOT);
                }
                else {
                    return leaf;
                }
                break;
            }
            else if(is_grp_open(current_token)){
                Slice grp_body_slice = cut_group_slice_to_size(new_slice(slice.arr, current_token_pos, slice.end));
                Node * node = build_node(grp_body_slice, in_scope_named_objects, in_scope_named_objects_count);
                if(nots != 0){
                    return create_sprout(node, MODIFIER_NOT);
                }
                else {
                    return node;
                }
                break;
            }
        }
        err("while building tree: expected a valid token");
        return NULL;
}

// TODO refactor
Node * build_node(Slice slice, NamedObject ** parent_named_objects, int parent_named_objects_count){

    NamedObject ** named_objects = malloc(sizeof(NamedObject*) * parent_named_objects_count);
    memcpy(named_objects, parent_named_objects, sizeof(NamedObject*) * parent_named_objects_count);
    int named_objects_count = parent_named_objects_count;

    int after_assignments_end = -1;

    Token temp_token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        temp_token = slice.arr[i];
        if(temp_token.type == SET){
            int identifier_pos = i+1;
            Slice body_slice = find_assignment_body(new_slice(slice.arr, i, slice.end));
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
        if(is_allowed_token_after_assignment_end(temp_token)){
            after_assignments_end = i;
            break;
        }
    }
    if(after_assignments_end == -1){
        err_at("while building tree: nothing to evaluate", i);
    }

    int combinator_pos = combinator_position(slice);
    if(combinator_pos == -1){
        Slice slice_without_assignments = new_slice(slice.arr, after_assignments_end,slice.end);
        return create_from_unforked_body(slice_without_assignments, named_objects , named_objects_count );
    }
    else {
        Slice left_slice = {slice.arr, after_assignments_end, combinator_pos - 1};
        Slice right_slice = {slice.arr, combinator_pos + 1, slice.end};
        Node * left = build_node(left_slice, named_objects, named_objects_count);
        Node * right = build_node(right_slice, named_objects, named_objects_count);
        Node * fork = create_fork(left, right, to_combinator(slice.arr[combinator_pos]));
        return fork;
    }
    err("while building tree");
    return NULL;
}


Node build_tree(Token * tokens, size_t start_token, size_t end_token){
    Slice slice = new_slice(tokens, start_token, end_token);

    NamedObject ** initial_named_objects = NULL;
    int initial_named_objects_count = 0;

    return *build_node(slice, initial_named_objects, initial_named_objects_count);
}
