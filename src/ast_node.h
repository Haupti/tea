#ifndef AST_NODE_HEADER
#define AST_NODE_HEADER

#include <stdlib.h>
#include "token.h"

#define NODE_TYPE_STR(x) (x == LEAF ? "LEAF" : (x == FORK ? "FORK" : (x == SPROUT ? "SPROUT" : (x == NAMED_LEAF ? "NAMED_LEAF" : ""))))

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

struct Node;

typedef struct Leaf {
    enum Value value;
} Leaf;

typedef enum NamedLeafType {
    LT_CONSTANT,
} NamedLeafType;

typedef struct NamedLeaf {
    NamedLeafType type;
    char * name;
    struct NamedLeaf ** in_scope_named_leafs;
    int in_scope_named_leafs_count;
} NamedLeaf;

typedef struct Sprout {
    struct Node * tip;
    enum Modifier modifier;
} Sprout;

typedef struct Fork {
    struct Node * left;
    struct Node * right;
    enum Combinator combinator;
} Fork;

typedef struct Conditional {
    struct NamedLeaf ** in_scope_named_leafs;
    int in_scope_named_leafs_count;
    struct Node * condition;
    struct Node * then;
    struct Node * otherwise;
} Conditional;

union NodeI {
    struct Leaf leaf;
    struct NamedLeaf named_leaf;
    struct Sprout sprout;
    struct Fork fork;
    struct Conditional condition;
};


typedef enum NodeType {
    LEAF,
    NAMED_LEAF,
    FORK,
    SPROUT,
} NodeType;

typedef struct Node {
    enum NodeType type;
    union NodeI it;
} Node;


Node * create_leaf(Value value);
Node * create_named_leaf(char * identifier_name, NamedLeaf ** in_scope_named_objects, int in_scope_named_objects_count);
Node * create_sprout(Node * leaf, Modifier modifier);
Node * create_fork(Node * left, Node * right, Combinator combinator);
Node new_leaf(Value value);
Node new_named_leaf(char * identifier_name, NamedLeaf ** in_scope_named_objects, int in_scope_named_objects_count);
Node new_sprout(Node * tip, Modifier modifier);
Node new_fork(Node * left, Node * right, Combinator combinator);

NamedLeaf new_constant(char * name, Node * node);
void print_named_object(NamedLeaf object);

void print_tree(Node node);

#endif
