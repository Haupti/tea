#ifndef INTERPRETER_HEADER
#define INTERPRETER_HEADER

#include <stdlib.h>

#include "ast_node.h"
#include "token.h"


Node build_tree(Token * tokens, size_t start_token, size_t end_token);

#endif
