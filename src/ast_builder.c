#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "ast_node.h"
#include "err.h"
#include "slice.h"
#include "slice_utils.h"
#include "bracket_finder.h"


typedef struct BuildScope {
    NodeReference ** node_refs;
    int node_refs_count;
    char ** scope_params;
    int scope_params_count;
    Function ** functions;
    int functions_count;
} BuildScope;

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

Node * build_node(Slice slice, BuildScope build_scope);

Node * build_conditional_node(Slice slice, BuildScope build_scope){
    Slice if_then_body = find_bracket_slice(slice);
    Node * condition_node = build_node(if_then_body, build_scope);
    slice.start = if_then_body.end + 1;
    Slice then_else_body = find_bracket_slice(slice);
    Node * then_node = build_node(then_else_body, build_scope);
    slice.start = then_else_body.end + 1;
    Slice else_end_body = find_bracket_slice(slice);
    Node * else_node = build_node(else_end_body, build_scope);
    Node * conditional = create_conditional(condition_node, then_node, else_node);
    return conditional;
}

int is_function_from_scope(char * name, BuildScope build_scope){
    for(int i = 0; i < build_scope.functions_count; i++){
        if(strcmp(name, build_scope.functions[i]->function_identifier) == 0){
            return 1;
        }
    }
    return 0;
}
int find_matching_grp_close_pos(Slice slice){
    if(!is_grp_open(slice.arr[slice.start])){
        err("while searching for closing bracket, im not at the start of a group");
    }
    int bracket_counter = 0;
    int i;
    for(i = slice.start; i <= slice.end; i++){
        if(is_grp_open(slice.arr[i])){
            bracket_counter += 1;
        }
        else if(is_grp_close(slice.arr[i])){
            bracket_counter -= 1;
        }
        if(bracket_counter == 0) {
            break;
        }
    }
    return i;
}

Slice get_next_param_slice(Slice params_start_slice){
    Token current_token;
    for(int i = params_start_slice.start; i <= params_start_slice.end; i++){
        current_token = params_start_slice.arr[i];
        if(is_value(current_token) || is_identifier(current_token)){
            Slice param_slice = {params_start_slice.arr, params_start_slice.start, i};
            return param_slice;
        }
        else if(is_grp_open(current_token)){
            Slice grp_open_slice = {params_start_slice.arr, i, params_start_slice.end};
            int grp_close_pos = find_matching_grp_close_pos(grp_open_slice);
            Slice param_slice = {params_start_slice.arr, params_start_slice.start, grp_close_pos};
            return param_slice;
        }
    }
    err("while collecting parameters for function call. unexpected end");
    Slice err = {NULL, 0, 0};
    return err;
}


// expects slice to only contain tokens of the node to create i.e. the assignments/definitions must be collected before calling this
Node * create_from_unforked_body(Slice slice, BuildScope build_scope){
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
            Node * created_node = NULL;
            if(is_function_from_scope(current_token.name, build_scope)){
                Node ** params = NULL;
                int params_count = 0;
                // collect params
                Slice current_slice = {slice.arr, current_token_pos+1, slice.end};
                while(1){
                    Slice param_slice = get_next_param_slice(current_slice);
                    current_slice.start = param_slice.end + 1;
                    Node * param = build_node(param_slice, build_scope);
                    params_count += 1;
                    Node ** tmp_params = realloc(params, sizeof(Node*) * params_count);
                    if(tmp_params == NULL){
                        err("cannot allocate space for param");
                    }
                    params = tmp_params;
                    params[params_count-1] = param;
                    if(param_slice.end == slice.end){
                        break;
                    }
                }
                FunctionScope * function_scope = create_function_scope(build_scope.functions, build_scope.functions_count);
                created_node = create_function_call_node(current_token.name, params, params_count, function_scope);
            } else { // is constant
                LeafScope * leaf_scope = create_leaf_scope(build_scope.node_refs, build_scope.node_refs_count, build_scope.scope_params, build_scope.scope_params_count);
                created_node = create_identifier_leaf(current_token.name, leaf_scope);
            }

            if(nots != 0){
                return create_sprout(created_node, MODIFIER_NOT);
            }
            else {
                return created_node;
            }
            break;
        }
        else if(is_grp_open(current_token)){
            Slice grp_body_slice = find_bracket_slice(new_slice(slice.arr, current_token_pos, slice.end));
            Node * node = build_node(grp_body_slice, build_scope);
            if(nots != 0){
                return create_sprout(node, MODIFIER_NOT);
            }
            else {
                return node;
            }
            break;
        }
        else if(current_token.type == IF){
            Slice cond_start_slice = {slice.arr, current_token_pos, slice.end};
            Node * cond_node =  build_conditional_node(cond_start_slice, build_scope);
            if(nots != 0){
                return create_sprout(cond_node, MODIFIER_NOT);
            }
            else {
                return cond_node;
            }
            break;
        }
    }
    err("while building tree: expected a valid token");
    return NULL;
}

Function * create_function_within(Slice slice, int * fn_def_end_target, BuildScope build_scope){

    //  collect params
    Slice function_name_params_slice = find_bracket_slice(slice);
    int params_count = (function_name_params_slice.end - function_name_params_slice.start);
    char * params[params_count];
    int params_index = 0;
    // add new params
    for(int i = function_name_params_slice.start + 1; i <= function_name_params_slice.end; i++){
        params[params_index] = function_name_params_slice.arr[i].name;
        params_index += 1;
    }

    // build body node
    Slice slice_from_define_as = {slice.arr, function_name_params_slice.end + 1, slice.end};
    Slice function_body_slice = find_bracket_slice(slice_from_define_as);
    *fn_def_end_target = function_body_slice.end + 1;
    Node * body = build_node(function_body_slice, build_scope);

    return create_function(function_name_params_slice.arr[function_name_params_slice.start].name, params, params_count, body);
}


int is_allowed_token_after_assignments(Token token){
    return is_value(token) || token.type == NOT || token.type == IDENTIFIER || is_grp_open(token) || token.type == IF;
}

Node * build_node(Slice slice, BuildScope build_scope){

    // copying of node refs and functions in scope
    NodeReference ** node_references = NULL;
    if(build_scope.node_refs_count != 0){
        node_references = malloc(sizeof(NodeReference*) * build_scope.node_refs_count);
        memcpy(node_references, build_scope.node_refs, sizeof(NodeReference*) * build_scope.node_refs_count);
    }
    int node_references_count = build_scope.node_refs_count;

    Function ** functions = NULL;
    if(build_scope.functions_count != 0){
        functions = malloc(sizeof(Function*) * build_scope.functions_count);
        memcpy(functions, build_scope.functions, sizeof(Function*) * build_scope.functions_count);
    }
    int functions_count = build_scope.functions_count;


    // building node
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
            BuildScope scope_copy = {node_references, node_references_count, build_scope.scope_params, build_scope.scope_params_count, functions, functions_count};
            Node * ref =  build_node(body_slice, scope_copy);

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
        if(temp_token.type == DEFINE){
            int identifier_pos = i+1;
            Slice slice_starting_at_define = {slice.arr, identifier_pos-1, slice.end};
            BuildScope scope_copy = {node_references, node_references_count, build_scope.scope_params, build_scope.scope_params_count, functions, functions_count};

            int fn_def_end = 0;
            Function * created_function = create_function_within(slice_starting_at_define, &fn_def_end, scope_copy);

            functions_count += 1;
            Function ** temp_functions = realloc(functions, sizeof(Function*) * (functions_count));
            if(temp_functions == NULL){
                err("failed to allocate space");
            }
            functions = temp_functions;
            functions[(functions_count)-1] = created_function;

            i = fn_def_end;
        }

        if(is_allowed_token_after_assignments(temp_token)){
            after_assignments_end = i;
            break;
        }
    }

    if(after_assignments_end == -1){
        err_at("while building tree: nothing to evaluate", i);
    }


    BuildScope scope_copy = {node_references, node_references_count, build_scope.scope_params, build_scope.scope_params_count, functions, functions_count};
    Slice after_assigments_slice = {slice.arr, after_assignments_end, slice.end};
    int combinator_pos = combinator_position(after_assigments_slice);
    if(combinator_pos == -1){
        Slice slice_without_assignments = new_slice(slice.arr, after_assignments_end, slice.end);
        return create_from_unforked_body(slice_without_assignments, scope_copy);
    }
    else {
        Slice left_slice = {slice.arr, after_assignments_end, combinator_pos - 1};
        Slice right_slice = {slice.arr, combinator_pos + 1, slice.end};
        Node * left = build_node(left_slice, scope_copy);
        Node * right = build_node(right_slice, scope_copy);
        Node * fork = create_fork(left, right, to_combinator(slice.arr[combinator_pos]));
        return fork;
    }
    err("while building tree");
    return NULL;
}


Node build_tree(Token * tokens, size_t start_token, size_t end_token){
    Slice slice = new_slice(tokens, start_token, end_token);

    BuildScope initial_scope = {NULL, 0, NULL, 0, NULL, 0};

    return *build_node(slice, initial_scope);
}
