#ifndef AST_NODE_HEADER
#define AST_NODE_HEADER

#include <stdlib.h>
#include "token.h"

#define NODE_TYPE_STR(x) (x == LEAF ? "LEAF" : (x == FORK ? "FORK" : (x == SPROUT ? "SPROUT" : "")))

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
    char * identifier;
} NodeValue;

typedef enum NodeType {
    LEAF,
    OBJECT_LEAF,
    FORK,
    SPROUT,
} NodeType;

struct NamedObject;

typedef struct Node {
    NodeType type;
    NodeValue value;
    struct Node * left;
    struct Node * right;
    struct Node * tip;
    struct NamedObject ** in_scope_named_objects;
    int in_scope_named_objects_count;
} Node;


typedef enum NamedObjectType {
    OT_CONSTANT,
} NamedObjectType;

typedef struct NamedObject {
    NamedObjectType type;
    char * name;
    Node * ref;
} NamedObject;

Node * create_leaf(Value value, NamedObject ** named_objects, int named_objects_count);
Node * create_object_leaf(char * identifier_name, NamedObject ** in_scope_named_objects, int in_scope_named_objects_count);
Node * create_sprout(Node * leaf, Modifier modifier);
Node * create_fork(Node * left, Node * right, Combinator combinator);
Node new_leaf(Value value, NamedObject ** in_scope_named_objects, int in_scope_named_objects_count);
Node new_object_leaf(char * identifier_name, NamedObject ** in_scope_named_objects, int in_scope_named_objects_count);
Node new_sprout(Node * tip, Modifier modifier);
Node new_fork(Node * left, Node * right, Combinator combinator);

NamedObject new_constant(char * name, Node * node);
void print_named_object(NamedObject object);

void print_tree(Node node);

#endif
