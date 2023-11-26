#include "ast_node.h"
#include <stdio.h>

Node new_leaf(Value value, NamedObject ** in_scope_named_objects, int in_scope_named_objects_count){
    NodeValue nodeValue;
    nodeValue.value = value;
    Node node = { LEAF, nodeValue, NULL, NULL, NULL, in_scope_named_objects, in_scope_named_objects_count};
    return node;
}

Node new_object_leaf(char * identifier_name, NamedObject ** in_scope_named_objects, int in_scope_named_objects_count){
    NodeValue nodeValue;
    nodeValue.identifier = identifier_name;
    Node node = { OBJECT_LEAF, nodeValue, NULL, NULL, NULL, in_scope_named_objects, in_scope_named_objects_count};
    return node;
}

Node new_fork(Node * left, Node * right, Combinator combinator){
    NodeValue nodeValue;
    nodeValue.combinator = combinator;
    Node node = { FORK, nodeValue, left, right, NULL, NULL, 0};
    return node;
}
Node new_sprout(Node * tip, Modifier modifier){
    NodeValue nodeValue;
    nodeValue.modifier = modifier;
    Node node = { SPROUT, nodeValue, NULL, NULL, tip, NULL, 0};
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
        case OBJECT_LEAF:
            {
                printf("IDENTIFIER(%s)", node.value.identifier);
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

NamedObject new_constant(char * name, Node * node){
    NamedObject named_object = { OT_CONSTANT , name, node };
    return named_object;
}

void print_named_object(NamedObject object){
    switch(object.type){
        case OT_CONSTANT:
            printf("NAMED OBJECT: constant { name = %s, ref = %s }", object.name, NODE_TYPE_STR(object.ref->type));
            break;
    }
}

