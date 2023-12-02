#ifndef AST_NODE_HEADER
#define AST_NODE_HEADER

#include "token.h"

#define NODE_TYPE_STR(x) (x == LEAF ? "LEAF" : (x == FORK ? "FORK" : (x == SPROUT ? "SPROUT" : (x == IDENTIFIER_LEAF ? "IDENTIFIER_LEAF" : ""))))

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


typedef enum NodeReferenceType {
    REF_TYPE_CONSTANT,
} NodeReferenceType;

// this is not a node
typedef struct NodeReference {
    NodeReferenceType type;
    char * name;
    struct Node * ref;
} NodeReference;

typedef struct Leaf {
    enum Value value;
} Leaf;

typedef struct IdentifierLeaf {
    char * name;
    struct NodeReference ** in_scope_node_refs;
    int in_scope_node_refs_count;
} IdentifierLeaf;

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
    struct Node * condition;
    struct Node * then;
    struct Node * otherwise;
} Conditional;

union NodeI {
    struct Leaf leaf;
    struct IdentifierLeaf id_leaf;
    struct Sprout sprout;
    struct Fork fork;
    struct Conditional conditional;
};

typedef enum NodeType {
    LEAF,
    IDENTIFIER_LEAF,
    FORK,
    SPROUT,
    CONDITIONAL,
} NodeType;

typedef struct Node {
    enum NodeType type;
    union NodeI it;
} Node;


Node * create_conditional(Node * condition, Node * then, Node * otherwise);
Node * create_leaf(Value value);
Node * create_identifier_leaf(char * identifier_name, NodeReference ** in_scope_node_refs, int in_scope_refs_count);
Node * create_sprout(Node * leaf, Modifier modifier);
Node * create_fork(Node * left, Node * right, Combinator combinator);
Node new_conditional(Node * condition, Node * then, Node * otherwise);
Node new_leaf(Value value);
Node new_identifier_leaf(char * identifier_name, NodeReference ** in_scope_node_refs, int in_scope_node_refs_count);
Node new_sprout(Node * tip, Modifier modifier);
Node new_fork(Node * left, Node * right, Combinator combinator);

NodeReference * create_node_ref(char * name, Node * ref);
NodeReference new_node_ref(char * name, Node * node);
void print_identifier_leaf(IdentifierLeaf object);

void print_tree(Node node);

#endif
