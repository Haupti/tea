#ifndef AST_EVALUATE_HEADER
#define AST_EVALUATE_HEADER

#include "ast_node.h"

typedef struct {
    Node ** params;
    int params_count;
} ParamScope;

Value evaluate_node(Node * node, ParamScope param_scope);

#endif
