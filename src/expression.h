#ifndef EXPRESSION_HEADER
#define EXPRESSION_HEADER

#include <stdlib.h>
#include "token.h"

typedef enum Value {
    VALUE_ON,
    VALUE_OFF
} Value;

typedef enum Combinator {
    COMBINATOR_AND,
    COMBINATOR_OR
} Combinator;


typedef union NodeValue{
    enum Value value;
    enum Combinator combinator;
} NodeValue;

typedef enum NodeType {
    LEAF,
    FORK,
} NodeType;

typedef struct Node {
    NodeType type;
    NodeValue value;
    struct Node * left;
    struct Node * right;
} Node;

Node new_leaf(Value value);
Node new_node(Node * left, Node * right, Combinator combinator);

void print_tree(Node node);

#endif
