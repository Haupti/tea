#include "expression.h"
#include <stdio.h>

Node new_leaf(Value value){
    NodeValue nodeValue;
    nodeValue.value = value;
    Node node = { LEAF, nodeValue, NULL, NULL };
    return node;
}

Node new_node(Node * left, Node * right, Combinator combinator){
    NodeValue nodeValue;
    nodeValue.combinator = combinator;
    Node node = { FORK, nodeValue, left, right};
    return node;
}

void print_tree(Node node){
    switch(node.type){
        case LEAF:
            switch(node.value.value){
                case VALUE_ON:
                    printf("VALUE_ON\n");
                case VALUE_OFF:
                    printf("VALUE_OFF\n");
            }
        case FORK:
            print_tree(*node.left);
            switch(node.value.combinator){
                case COMBINATOR_AND:
                    printf(" AND ");
                case COMBINATOR_OR:
                    printf(" OR ");
            }
            print_tree(*node.right);
    }
}
