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

int is_allowed_token_after_assignments(Token token){
    return is_value(token) || token.type == NOT || token.type == IDENTIFIER || is_grp_open(token);
}

Node * build_node(Slice slice, NodeReference ** parent_node_references, int parent_node_references_count);

// expects slice to only contain tokens of the node to create i.e. the assignments must be collected before calling this
Node * create_from_unforked_body(Slice slice, NodeReference ** in_scope_node_references, int in_scope_node_references_count){
        Token current_token;
        int nots = 0;
        int current_token_pos;
        for(current_token_pos = slice.start; current_token_pos <= slice.end; current_token_pos++){
            current_token = slice.arr[current_token_pos];
            if(current_token.type == NOT){
                nots = (nots + 1) % 2;
            }
            else if(is_value(current_token)) {
                Node * leaf = create_leaf(to_value(current_token));
                if(nots != 0){
                    return create_sprout(leaf, MODIFIER_NOT);
                }
                else {
                    return leaf;
                }
                break;
            }
            else if(is_identifier(current_token)){
                Node * leaf = create_identifier_leaf(current_token.name, in_scope_node_references, in_scope_node_references_count);
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
                Node * node = build_node(grp_body_slice, in_scope_node_references, in_scope_node_references_count);
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


Node * build_node(Slice slice, NodeReference ** parent_node_references, int parent_node_references_count){

    NodeReference ** node_references = malloc(sizeof(NodeReference*) * parent_node_references_count);
    memcpy(node_references, parent_node_references, sizeof(NodeReference*) * parent_node_references_count);
    int node_references_count = parent_node_references_count;

    int after_assignments_end = -1;

    Token temp_token;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        temp_token = slice.arr[i];
        if(temp_token.type == SET){
            int identifier_pos = i+1;
            Slice slice_starting_at_set = {slice.arr, identifier_pos-1, slice.end};


            Token identifier = slice_starting_at_set.arr[slice_starting_at_set.start+1];
            Slice body_slice = find_assignment_body(slice_starting_at_set);
            Node * ref =  build_node(body_slice, node_references, node_references_count);

            NodeReference * node_ref = create_node_ref(identifier.name, ref) ;

            node_references_count += 1;
            NodeReference ** temp_node_references = realloc(node_references, sizeof(NodeReference*) * (node_references_count));
            if(temp_node_references == NULL){
                err("failed to allocate space");
            }
            node_references = temp_node_references;
            node_references[(node_references_count)-1] = node_ref;
            int assignment_end_pos =  body_slice.end + 1;
            i = assignment_end_pos;
        }

        if(is_allowed_token_after_assignments(temp_token)){
            after_assignments_end = i;
            break;
        }
    }

    if(after_assignments_end == -1){
        err_at("while building tree: nothing to evaluate", i);
    }

    Slice after_assigments_slice = {slice.arr, after_assignments_end, slice.end};
    int combinator_pos = combinator_position(after_assigments_slice);
    if(combinator_pos == -1){
        Slice slice_without_assignments = new_slice(slice.arr, after_assignments_end,slice.end);
        return create_from_unforked_body(slice_without_assignments, node_references , node_references_count );
    }
    else {
        Slice left_slice = {slice.arr, after_assignments_end, combinator_pos - 1};
        Slice right_slice = {slice.arr, combinator_pos + 1, slice.end};
        Node * left = build_node(left_slice, node_references, node_references_count);
        Node * right = build_node(right_slice, node_references, node_references_count);
        Node * fork = create_fork(left, right, to_combinator(slice.arr[combinator_pos]));
        return fork;
    }
    err("while building tree");
    return NULL;
}


Node build_tree(Token * tokens, size_t start_token, size_t end_token){
    Slice slice = new_slice(tokens, start_token, end_token);

    NodeReference ** initial_node_references = NULL;
    int initial_node_references_count = 0;

    return *build_node(slice, initial_node_references, initial_node_references_count);
}
