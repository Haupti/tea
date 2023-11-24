#ifndef AST_BUILDER_INTERNAL_HEADER
#define AST_BUILDER_INTERNAL_HEADER

#include "../src/cursor.h"
#include "../src/slice.h"
#include "../src/ast_node.h"

Cursor left_expression_cursor(Cursor * original_cursor);
Cursor right_expression_cursor(Cursor * original_cursor);
int find_matching_combinator(Cursor * original_cursor);
int holds_a_sprout(Cursor * original_cursor);


Slice cut_group_slice_to_size(Slice slice);
int effective_not_count(Slice slice);
Node * to_leaf_or_spout_with_leaf(Slice slice);

#endif
