#include "ast_node.h"
#include <stdio.h>
#include <string.h>
#include "err.h"

FunctionScope * create_function_scope(Function ** functions, int functions_count){
    FunctionScope scope = { functions, functions_count };
    FunctionScope * ptr = malloc(sizeof(FunctionScope));
    if(ptr == NULL){
        err("cannot allocate space for scope");
    }
    memcpy(ptr, &scope , sizeof(FunctionScope));
    return ptr;
}

LeafScope * create_leaf_scope(NodeReference ** node_refs, int node_refs_count, char ** function_params, int function_params_count){
    LeafScope scope = { node_refs, node_refs_count, function_params, function_params_count};
    LeafScope * ptr = malloc(sizeof(LeafScope));
    if(ptr == NULL){
        err("cannot allocate space for scope");
    }
    memcpy(ptr, &scope , sizeof(LeafScope));
    return ptr;
}
Node * create_function_call_node(char * name, Node ** params, int params_count, FunctionScope * function_scope){
    Node node = new_function_call_node(name, params, params_count, function_scope);
    Node * ptr = malloc(sizeof(Node));
    if(ptr == NULL){
        err("cannot allocate space for node");
    }
    memcpy(ptr, &node , sizeof(Node));
    return ptr;
}

NodeReference * create_node_ref(char * name, Node * ref){
    NodeReference node_ref = new_node_ref(name, ref);
    NodeReference * node_ref_ptr = malloc(sizeof(NodeReference));
    if(node_ref_ptr == NULL){
        err("cannot allocate space for node");
    }
    memcpy(node_ref_ptr, &node_ref , sizeof(NodeReference));
    return node_ref_ptr;
}

Function * create_function(char * function_identifier, char ** arg_names, int args_count, Node * body){
    Function function = {arg_names, args_count, function_identifier, body};
    Function * ptr = malloc(sizeof(Function));
    if(ptr == NULL){
        err("cannot allocate space for node");
    }
    memcpy(ptr, &function , sizeof(Function));
    return ptr;
}

NodeReference new_node_ref(char * name, Node * node){
    NodeReference named_object = { REF_TYPE_CONSTANT, name, node };
    return named_object;
}

Node * create_conditional(Node * condition, Node * then, Node * otherwise){
    Node conditional = new_conditional(condition, then, otherwise);
    Node * conditional_ptr = malloc(sizeof(Node));
    if(conditional_ptr == NULL){
        err("cannot allocate space for node");
    }
    memcpy(conditional_ptr, &conditional, sizeof(Node));
    return conditional_ptr;
}

Node * create_fork(Node * left, Node * right, Combinator combinator){
    Node fork = new_fork(left, right, combinator);
    Node * fork_ptr = malloc(sizeof(Node));
    if(fork_ptr == NULL){
        err("cannot allocate space for node");
    }
    memcpy(fork_ptr, &fork, sizeof(Node));
    return fork_ptr;
}

Node * create_identifier_leaf(char * identifier_name, LeafScope * leaf_scope){
    Node leaf = new_identifier_leaf(identifier_name, leaf_scope);
    Node * leaf_ptr = malloc(sizeof(Node));
    if(leaf_ptr == NULL){
        err("cannot allocate space for node");
    }
    memcpy(leaf_ptr, &leaf, sizeof(Node));
    return leaf_ptr;
}

Node * create_leaf(Value value){
    Node leaf = new_leaf(value);
    Node * leaf_ptr = malloc(sizeof(Node));
    if(leaf_ptr == NULL){
        err("cannot allocate space for node");
    }
    memcpy(leaf_ptr, &leaf, sizeof(Node));
    return leaf_ptr;
}

Node * create_sprout(Node * leaf, Modifier modifier){
    Node sprout = new_sprout(leaf, modifier);
    Node * sprout_ptr = malloc(sizeof(Node));
    if(sprout_ptr == NULL){
        err("cannot allocate space for node");
    }
    memcpy(sprout_ptr, &sprout, sizeof(Node));
    return sprout_ptr;
}


Node new_function_call_node(char * name, Node ** params, int params_count, FunctionScope * function_scope){
    FunctionCallNode function_call_node = { name, params, params_count, function_scope};
    union NodeI node_i;
    node_i.function_call = function_call_node;
    Node node = { FUNCTION_CALL_NODE, node_i };
    return node;
}

Node new_conditional(Node * condition, Node * then, Node * otherwise){
    Conditional conditional = { condition, then, otherwise };
    union NodeI node_i;
    node_i.conditional = conditional;
    Node node = { CONDITIONAL, node_i };
    return node;
}

Node new_leaf(Value value){
    Leaf leaf = { value };
    union NodeI node_i;
    node_i.leaf = leaf;
    Node node = { LEAF, node_i };
    return node;
}

Node new_identifier_leaf(char * identifier_name, LeafScope * leaf_scope){
    IdentifierLeaf identifier_leaf = { identifier_name, leaf_scope };
    union NodeI node_i;
    node_i.id_leaf = identifier_leaf;
    Node node = { IDENTIFIER_LEAF, node_i};
    return node;
}

Node new_fork(Node * left, Node * right, Combinator combinator){
    Fork fork = { left, right, combinator };
    union NodeI node_i;
    node_i.fork = fork;
    Node node = { FORK, node_i};
    return node;
}
Node new_sprout(Node * tip, Modifier modifier){
    Sprout sprout = { tip, modifier };
    union NodeI node_i;
    node_i.sprout = sprout;
    Node node = { SPROUT, node_i};
    return node;
}

void print_tree(Node node){
    switch(node.type){
        case LEAF:
            {
                Leaf leaf = node.it.leaf;
                switch(leaf.value){
                    case VALUE_ON:
                        printf("VALUE_ON");
                        break;
                    case VALUE_OFF:
                        printf("VALUE_OFF");
                        break;
                }
                break;
            }
        case IDENTIFIER_LEAF:
            {
                IdentifierLeaf id_leaf = node.it.id_leaf;
                printf("IDENTIFIER(%s)", id_leaf.name);
                break;
            }
        case SPROUT:
            {
                Sprout sprout = node.it.sprout;
                if(sprout.modifier == MODIFIER_NOT){
                    printf("!(");
                }
                else {
                    printf("(");
                }
                print_tree(*sprout.tip);
                printf(")");
                break;
            }
        case FORK:
            {
                Fork fork = node.it.fork;
                print_tree(*fork.left);
                switch(fork.combinator){
                    case COMBINATOR_AND:
                        printf(" AND ");
                        break;
                    case COMBINATOR_OR:
                        printf(" OR ");
                        break;
                }
                print_tree(*fork.right);
                break;
            }
        case CONDITIONAL:
            {
                Conditional cond = node.it.conditional;
                printf("IF ");
                print_tree(*cond.condition);
                printf("THEN ");
                print_tree(*cond.then);
                printf("ELSE ");
                print_tree(*cond.otherwise);
                printf(" END");
                break;
            }
        case FUNCTION_CALL_NODE:
            {
                FunctionCallNode call = node.it.function_call;
                printf("(FUNCTION CALL %s)", call.function_identifier);
                break;
            }
    }
}

void print_node_reference(NodeReference object){
    switch(object.type){
        case REF_TYPE_CONSTANT:
            printf("REFERENCE: constant { name = %s, ref = %s }", object.name, NODE_TYPE_STR(object.ref->type));
            break;
    }
}

