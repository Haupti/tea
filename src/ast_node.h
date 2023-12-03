#ifndef AST_NODE_HEADER
#define AST_NODE_HEADER

#include "token.h"

#define NODE_TYPE_STR(x) (x == LEAF ? "LEAF" : (x == FORK ? "FORK" : (x == SPROUT ? "SPROUT" : (x == IDENTIFIER_LEAF ? "IDENTIFIER_LEAF" : (x == FUNCTION_CALL_NODE ? "FUNCTION_CALL_NODE" : "")))))

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
struct LeafScope;
struct FunctionScope;


typedef enum NodeReferenceType {
    REF_TYPE_CONSTANT,
} NodeReferenceType;

// this is not a node
typedef struct NodeReference {
    NodeReferenceType type;
    char * name;
    struct Node * ref;
} NodeReference;

typedef struct Function {
    char ** arg_names;
    int args_count;
    char * function_identifier;
    struct Node * body;
} Function;

typedef struct Leaf {
    enum Value value;
} Leaf;

typedef struct IdentifierLeaf {
    char * name;
    struct LeafScope * leaf_scope;
} IdentifierLeaf;

typedef struct FunctionCallNode {
    char * function_identifier;
    struct Node ** params;
    int params_count;
    struct FunctionScope * function_scope;
} FunctionCallNode;

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
    struct FunctionCallNode function_call;
};

typedef enum NodeType {
    LEAF,
    IDENTIFIER_LEAF,
    FORK,
    SPROUT,
    CONDITIONAL,
    FUNCTION_CALL_NODE,
} NodeType;

typedef struct Node {
    enum NodeType type;
    union NodeI it;
} Node;

typedef struct LeafScope {
    NodeReference ** scope_node_references;
    int scope_node_references_count;
    char ** function_scope_params;
    int function_scope_params_count;
} LeafScope;

typedef struct FunctionScope {
    struct Function ** functions;
    int functions_count;
} FunctionScope;


Node * create_conditional(Node * condition, Node * then, Node * otherwise);
Node * create_leaf(Value value);
Node * create_identifier_leaf(char * identifier_name, LeafScope * leaf_scope);
Node * create_sprout(Node * leaf, Modifier modifier);
Node * create_fork(Node * left, Node * right, Combinator combinator);
Node * create_function_call_node(char * name, Node ** params, int params_count, FunctionScope * function_scope);

Node new_function_call_node(char * name, Node ** params, int params_count, FunctionScope * function_scope);
Node new_conditional(Node * condition, Node * then, Node * otherwise);
Node new_leaf(Value value);
Node new_identifier_leaf(char * identifier_name, LeafScope * leaf_scope);
Node new_sprout(Node * tip, Modifier modifier);
Node new_fork(Node * left, Node * right, Combinator combinator);

LeafScope * create_leaf_scope(NodeReference ** node_refs, int node_refs_count, char ** function_params, int function_params_count);
FunctionScope * create_function_scope(Function ** functions, int functions_count);
Function * create_function(char * function_identifier, char ** arg_names, int args_count, Node * body);
NodeReference * create_node_ref(char * name, Node * ref);
NodeReference new_node_ref(char * name, Node * node);
void print_identifier_leaf(IdentifierLeaf object);

void print_tree(Node node);

#endif
