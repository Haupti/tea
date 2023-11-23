#include "ast_node.h"
#include <stdio.h>

Node new_leaf(Value value){
    NodeValue nodeValue;
    nodeValue.value = value;
    Node node = { LEAF, nodeValue, NULL, NULL, NULL };
    return node;
}

Node new_fork(Node * left, Node * right, Combinator combinator){
    NodeValue nodeValue;
    nodeValue.combinator = combinator;
    Node node = { FORK, nodeValue, left, right, NULL};
    return node;
}
Node new_sprout(Node * tip, Modifier modifier){
    NodeValue nodeValue;
    nodeValue.modifier = modifier;
    Node node = { SPROUT, nodeValue, NULL, NULL, tip};
    return node;
}

void print_tree(Node node){
    switch(node.type){
        case LEAF:
            {
                switch(node.value.value){
                    case VALUE_ON:
                        printf("VALUE_ON");
                        break;
                    case VALUE_OFF:
                        printf("VALUE_OFF");
                        break;
                }
                break;
            }
        case SPROUT:
            {
                if(node.value.modifier == MODIFIER_NOT){
                    printf("!(");
                }
                else {
                    printf("(");
                }
                print_tree(*node.tip);
                printf(")");
                break;
            }
        case FORK:
            {
                print_tree(*node.left);
                switch(node.value.combinator){
                    case COMBINATOR_AND:
                        printf(" AND ");
                        break;
                    case COMBINATOR_OR:
                        printf(" OR ");
                        break;
                }
                print_tree(*node.right);
                break;
            }
    }
}
