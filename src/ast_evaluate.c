#include "ast_node.h"
#include "ast_evaluate.h"
#include "err.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Value evaluate_fork(Fork fork){
    Combinator combinator = fork.combinator;
    Value left;
    Value right;

    if(fork.left->type == LEAF){
        left = fork.left->it.leaf.value;
    }
    else {
        left = evaluate_node(fork.left);
    }

    if(fork.right->type == LEAF){
        right = fork.right->it.leaf.value;
    }
    else {
        right = evaluate_node(fork.right);
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

Value evaluate_leaf(Leaf leaf){
   return leaf.value;
}

Value evaluate_identifier_leaf(IdentifierLeaf leaf){
    NodeReference ** node_refs = leaf.in_scope_node_refs;
    int node_refs_count = leaf.in_scope_node_refs_count;

    for(int i = 0; i< node_refs_count; i++){
        if(strcmp(node_refs[i]->name, leaf.name) == 0){
            return evaluate_node(node_refs[i]->ref);
        }
    }
    printf("ERROR: '%s' is now known here\n", leaf.name);
    exit(EXIT_FAILURE);
}

Value evaluate_generic_leaf(Node * node){
    if(node->type == LEAF){
        return evaluate_leaf(node->it.leaf);
    }
    else if(node->type == IDENTIFIER_LEAF){
        return evaluate_identifier_leaf(node->it.id_leaf);
    }
    err("value evaluation impossible. this is no leaf");
    return VALUE_OFF;
}

Value evaluate_sprout(Sprout sprout){
    Modifier modifier = sprout.modifier;

    Value value;
    if(sprout.tip->type == LEAF){
        value = sprout.tip->it.leaf.value;
    }else {
        value = evaluate_node(sprout.tip);
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


Value evaluate_node(Node * node){
    switch(node->type){
        case IDENTIFIER_LEAF:
            return evaluate_generic_leaf(node);
            break;
        case LEAF:
            return evaluate_generic_leaf(node);
            break;
        case SPROUT:
            return evaluate_sprout(node->it.sprout);
            break;
        case FORK:
            return evaluate_fork(node->it.fork);
            break;
    }
    err("malformed tree");
    return -1;
}
