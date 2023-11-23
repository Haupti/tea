#ifndef AST_NODE_HEADER
#define AST_NODE_HEADER

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


typedef enum Modifier {
    MODIFIER_ID,
    MODIFIER_NOT,
} Modifier;

typedef union NodeValue{
    enum Value value;
    enum Combinator combinator;
    enum Modifier modifier;
} NodeValue;

typedef enum NodeType {
    LEAF,
    FORK,
    SPROUT,
} NodeType;

typedef struct Node {
    NodeType type;
    NodeValue value;
    struct Node * left;
    struct Node * right;
    struct Node * tip;
} Node;

Node new_leaf(Value value);
Node new_sprout(Node * tip, Modifier modifier);
Node new_fork(Node * left, Node * right, Combinator combinator);

/* TODO consider this later, if this becomes useful
typedef struct Leaf {
    Value value;
} Leaf;

typedef struct Sprout{
    Modifier modifier;
    Node * tip;
} Sprout;

typedef struct Fork{
    Combinator combinator;
    Node * left;
    Node * right;
} Fork;
as_leaf(Node * node);
as_sprout(Node * node);
as_fork(Node * node);
*/

void print_tree(Node node);

#endif
