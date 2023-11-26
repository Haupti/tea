#include "ast_node.h"
#include "ast_evaluate.h"
#include "err.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Value evaluate_fork(Combinator combinator, Value left, Value right){
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

Value evaluate_leaf(Node * node){
    NamedObject ** in_scope_named_objects = node->in_scope_named_objects;
    int object_count = node->in_scope_named_objects_count;

    if(node->type == LEAF){
        return node->value.value;
    }
    else if(node->type == OBJECT_LEAF){
        for(int i = 0; i< object_count; i++){
            if(strcmp(in_scope_named_objects[i]->name, node->value.identifier) == 0){
                return evaluate_node(in_scope_named_objects[i]->ref);
            }
        }
    }
    err("value evaluation impossible (leaf)");
    return VALUE_OFF;
}

Value evaluate_sprout(Modifier modifier, Value value){
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
        case OBJECT_LEAF:
            return evaluate_leaf(node);
            break;
        case LEAF:
            return node->value.value;
            break;
        case SPROUT:
            {
                if(node->tip->type == LEAF){
                    return evaluate_sprout(node->value.modifier, node->tip->value.value);
                }else {
                    return evaluate_sprout(node->value.modifier, evaluate_node(node->tip));
                }
                break;
            }
        case FORK:
            {
                if(node->left->type == LEAF && node->right->type == LEAF){
                    return evaluate_fork(node->value.combinator, node->left->value.value, node->right->value.value);
                }
                if(node->left->type == LEAF && node->right->type != LEAF){
                    return evaluate_fork(node->value.combinator, node->left->value.value, evaluate_node(node->right));
                }
                if(node->left->type != LEAF && node->right->type == LEAF){
                    return evaluate_fork(node->value.combinator, evaluate_node(node->left), node->right->value.value);
                }
                if(node->left->type != LEAF && node->right->type != LEAF){
                    return evaluate_fork(node->value.combinator, evaluate_node(node->left), evaluate_node(node->right));
                }
                break;
            }
    }
    err("malformed tree");
    return -1;
}
