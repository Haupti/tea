#include "ast_node.h"
#include "ast_evaluate.h"

Value evaluate_end(Combinator combinator, Value left, Value right){
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
}

Value evaluate_node(Node * node){
    switch(node->type){
        case LEAF:
            return node->value.value;
            break;
        case FORK:
            if(node->left->type == LEAF && node->right->type == LEAF){
                return evaluate_end(node->value.combinator, node->left->value.value, node->right->value.value);
            }
            if(node->left->type == LEAF && node->right->type == FORK){
                return evaluate_end(node->value.combinator, node->left->value.value, evaluate_node(node->right));
            }
            if(node->left->type == FORK && node->right->type == LEAF){
                return evaluate_end(node->value.combinator, evaluate_node(node->left), node->right->value.value);
            }
            if(node->left->type == FORK && node->right->type == FORK){
                return evaluate_end(node->value.combinator, evaluate_node(node->left), evaluate_node(node->right));
            }
    }
}
