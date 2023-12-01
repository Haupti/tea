#include "ast_node.h"
#include <stdio.h>
#include <string.h>
#include "err.h"

Node * create_fork(Node * left, Node * right, Combinator combinator){
    Node fork = new_fork(left, right, combinator);
    Node * fork_ptr = malloc(sizeof(Node));
    if(fork_ptr == NULL){
        err("cannot allocate space for node");
    }
    memcpy(fork_ptr, &fork, sizeof(Node));
    return fork_ptr;
}


Node * create_object_leaf(char * identifier_name, NodeReference ** in_scope_node_refs, int in_scope_node_refs_count){
    Node leaf = new_named_leaf(identifier_name, in_scope_node_refs, in_scope_node_refs_count);
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

Node new_leaf(Value value){
    Leaf leaf = { value };
    union NodeI node_i;
    node_i.leaf = leaf;
    Node node = { LEAF, node_i };
    return node;
}

Node new_identifier_leaf(char * identifier_name, NodeReference ** in_scope_node_refs, int ins_scope_node_refs_count){
    IdentifierLeaf identifier_leaf = { identifier_name, in_scope_node_refs, ins_scope_node_refs_count };
    union NodeI node_i;
    node_i.id_leaf = identifier_leaf;
    Node node = { NAMED_LEAF, node_i};
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
        case NAMED_LEAF:
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
    }
}

NodeReference new_node_ref(char * name, Node * node){
    NodeReference named_object = { REF_TYPE_CONSTANT, name, node };
    return named_object;
}

void print_node_reference(NodeReference object){
    switch(object.type){
        case REF_TYPE_CONSTANT:
            printf("REFERENCE: constant { name = %s, ref = %s }", object.name, NODE_TYPE_STR(object.ref->type));
            break;
    }
}

