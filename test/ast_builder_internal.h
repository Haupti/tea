#ifndef AST_BUILDER_INTERNAL_HEADER
#define AST_BUILDER_INTERNAL_HEADER

#include "../src/slice.h"
#include "../src/ast_node.h"

Slice cut_group_slice_to_size(Slice slice);
int effective_not_count(Slice slice);
Node * to_leaf_or_spout_with_leaf(Slice slice);

#endif
