#include "ast_node.h"
#include "ast_evaluate.h"
#include "err.h"
#include "token.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Value evaluate_fork(Fork fork, ParamScope param_scope){
    Combinator combinator = fork.combinator;
    Value left;
    Value right;

    if(fork.left->type == LEAF){
        left = fork.left->it.leaf.value;
    }
    else {
        left = evaluate_node(fork.left, param_scope);
    }

    if(fork.right->type == LEAF){
        right = fork.right->it.leaf.value;
    }
    else {
        right = evaluate_node(fork.right, param_scope);
    }

    switch(combinator){
        case COMBINATOR_AND:
            {
                if(left == VALUE_ON && right == VALUE_ON){
                    return VALUE_ON;
                }
                else{
                    return VALUE_OFF;
                }
                break;
            }
        case COMBINATOR_OR:
            {
                if(left == VALUE_ON || right == VALUE_ON){
                    return VALUE_ON;
                }
                else{
                    return VALUE_OFF;
                }
                break;
            }
    }
    err("value evaluation impossible (fork)");
}

Value evaluate_conditional_node(Conditional conditional, ParamScope param_scope){
    Value cond = evaluate_node(conditional.condition, param_scope);
    if(cond == VALUE_ON){
        return evaluate_node(conditional.then, param_scope);
    }
    else {
        return evaluate_node(conditional.otherwise, param_scope);
    }
}

Value evaluate_leaf(Leaf leaf){
   return leaf.value;
}

Function * get_function_from_scope(char * function_identifier, FunctionScope * scope){
    for(int i = 0; i < scope->functions_count; i++){
        Function * current = scope->functions[i];
        if(strcmp(function_identifier, current->function_identifier) == 0){
            return current;
        }
    }
    return NULL;
}


Value evaluate_function_call(FunctionCallNode fn_node){
    Function * fn = get_function_from_scope(fn_node.function_identifier, fn_node.function_scope);
    if(fn == NULL){
        err("identifier of function is not known here");
    }
    Node ** params = fn_node.params;
    int params_count = fn_node.params_count;

    ParamScope param_scope = {params, params_count};

    return evaluate_node(fn->body, param_scope);
}

Value evaluate_identifier_leaf(IdentifierLeaf leaf, ParamScope param_scope){
    NodeReference ** node_refs = leaf.leaf_scope->scope_node_references;
    int node_refs_count = leaf.leaf_scope->scope_node_references_count;

    for(int i = 0; i < node_refs_count; i++){
        if(strcmp(node_refs[i]->name, leaf.name) == 0){
            return evaluate_node(node_refs[i]->ref, param_scope);
        }
    }
    printf("param_scope count: %d\n", param_scope.params_count);
    printf("param_scope param 1 == null: %d\n", param_scope.params[0] == NULL);
    printf("param_scope param 2 == null: %d\n", param_scope.params[1] == NULL);

    printf("leaf scope count: %d\n", leaf.leaf_scope->function_scope_params_count);
    printf("leaf scope param 1 == null: %d\n", leaf.leaf_scope->function_scope_params[0] == NULL);
    printf("leaf scope param 2 == null: %d\n", leaf.leaf_scope->function_scope_params[1] == NULL);
    for(int i = 0; i < param_scope.params_count; i++){
        if(strcmp(leaf.leaf_scope->function_scope_params[i], leaf.name) == 0){
            ParamScope empty = { NULL, 0};
            return evaluate_node(param_scope.params[i], empty);
        }
    }
    printf("ERROR: '%s' is now known here\n", leaf.name);
    exit(EXIT_FAILURE);
}

Value evaluate_generic_leaf(Node * node, ParamScope param_scope){
    if(node->type == LEAF){
        return evaluate_leaf(node->it.leaf);
    }
    else if(node->type == IDENTIFIER_LEAF){
        return evaluate_identifier_leaf(node->it.id_leaf, param_scope);
    }
    err("value evaluation impossible. this is no leaf");
    return VALUE_OFF;
}

Value evaluate_sprout(Sprout sprout, ParamScope param_scope){
    Modifier modifier = sprout.modifier;

    Value value;
    if(sprout.tip->type == LEAF){
        value = sprout.tip->it.leaf.value;
    }else {
        value = evaluate_node(sprout.tip, param_scope);
    }

    switch(modifier){
        case MODIFIER_ID:
            return value;
        case MODIFIER_NOT:
            switch(value){
                case VALUE_ON:
                    return VALUE_OFF;
                case VALUE_OFF:
                    return VALUE_ON;
            }
    }
}


Value evaluate_node(Node * node, ParamScope param_scope){
    switch(node->type){
        case IDENTIFIER_LEAF:
            return evaluate_generic_leaf(node, param_scope);
            break;
        case LEAF:
            return evaluate_generic_leaf(node, param_scope);
            break;
        case SPROUT:
            return evaluate_sprout(node->it.sprout, param_scope);
            break;
        case FORK:
            return evaluate_fork(node->it.fork, param_scope);
            break;
        case CONDITIONAL:
            return evaluate_conditional_node(node->it.conditional, param_scope);
            break;
        case FUNCTION_CALL_NODE:
            return evaluate_function_call(node->it.function_call);
            break;
        default:
            err("malformed tree");
            break;
    }
    err("malformed tree");
    return -1;
}
