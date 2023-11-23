#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "ast_node.h"
#include "cursor.h"
#include "err.h"


// expects the cursor to be at the position of grp_close
// returns value between start and end: the position of the matching grp_close
// error if no matching grp_close could be found
int find_matching_grp_close(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    if(current(cursor).type != GRP_OPEN){
        err("while searching bracket: cannot find matching ')' if im not at a '('");
    }
    Cursor temp = copy_cursor(cursor);
    Token token = current(&temp);
    int bracket_counter = 1;
    while(has_next(&temp)){
        token = next(&temp);
        switch(token.type){
            case GRP_OPEN:
                bracket_counter += 1;
                break;
            case GRP_CLOSE:
                bracket_counter -= 1;
                if(bracket_counter == 0){
                    return temp.position;
                }
                break;
            default:
                break;
        }
    }
    err("while searching bracket: no matching ')' found");
    return -1;
}



Value to_value(Token token){
    Value value;
    switch(token.type){
        case OFF:
            value = VALUE_OFF;
            break;
        case ON:
            value = VALUE_ON;
            break;
        default:
            err("while parsing: token is not a value");
            break;
    }
    return value;
}

Combinator to_combinator(Token token){
    Combinator combinator;
    switch(token.type){
        case AND:
            combinator = COMBINATOR_AND;
            break;
        case OR:
            combinator = COMBINATOR_OR;
            break;
        default:
            err("while parsing: token is not a combinator");
            break;
    }
    return combinator;
}

Cursor left_expression_cursor(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;
    const int start = cursor->position;
    int end;

    while(1){
        if(is_grp_open(current(cursor))){
            end = find_matching_grp_close(cursor);
            break;
        }
        if(is_value(current(cursor))){
            end = cursor->position;
            break;
        }
        next(cursor);
    }

    Cursor new_cursor = {cursor->arr, start, end, start};
    return new_cursor;
}


int find_matching_combinator(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    while(1){ // must find an combinator
        if(is_grp_open(current(cursor))){
            int matching_close = find_matching_grp_close(cursor);
            jump_to(cursor, matching_close);
            next(cursor);
            break;
        }
        if(is_value(current(cursor))){
            next(cursor);
            break;
        }
        next(cursor);
    }
    return cursor->position;
}


Combinator find_combinator(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    int combinator_pos = find_matching_combinator(cursor) ;
    Combinator combinator = to_combinator(cursor->arr[combinator_pos]);

    return combinator;
}

Cursor right_expression_cursor(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    int combinator_pos = find_matching_combinator(cursor);
    jump_to(cursor, combinator_pos);
    next(cursor); // at start right expression
    const int start = cursor->position;

    int end;
    while(1){ // must find an expression end
        if(is_grp_open(current(cursor))){
            end = find_matching_grp_close(cursor);
            break;
        }
        if(is_value(current(cursor))){
            end = cursor->position;
            break;
        }
        next(cursor);
    }

    Cursor new_cursor = {cursor->arr, start, end, start};
    return new_cursor;
}

Cursor create_cursor_for_inner_expession(Cursor * cursor){
   Cursor inner_cursor = new_cursor(cursor->arr, (cursor->start + 1), (cursor->end -1));
   return inner_cursor;
}

Cursor create_cursor_without_first(Cursor * cursor){
   Cursor inner_cursor = new_cursor(cursor->arr, (cursor->start + 1), cursor->end);
   return inner_cursor;
}

int holds_a_sprout(Cursor * original_cursor){
    Cursor cursor_val = copy_cursor(original_cursor);
    Cursor * cursor = &cursor_val;

    Token token = current(cursor);
    if(token.type == GRP_OPEN && find_matching_grp_close(cursor) == cursor->end){
        return 1;
    }
    else if(is_value(token)){
        return cursor->position == cursor->end;
    }
    else { // modifier or something else
        Cursor shrinked_cursor = create_cursor_without_first(cursor);
        return holds_a_sprout(&shrinked_cursor);
    }
}

Node * build_node(Cursor cursor){
    if(size(&cursor) == 1){
        Node * leaf = malloc(sizeof(Node));
        Node temp_leaf = new_leaf(to_value(current(&cursor)));
        memcpy(leaf, &temp_leaf, sizeof(Node));
        return leaf;
    }
    else if(holds_a_sprout(&cursor)){
        Modifier modifier;
        Cursor inner_cursor;
        if(current(&cursor).type == NOT){
            modifier = MODIFIER_NOT;
            inner_cursor = create_cursor_without_first(&cursor);
        }
        else { // must be a bracket then
            modifier = MODIFIER_ID;
            inner_cursor = create_cursor_for_inner_expession(&cursor);
        }
        Node * sprout = malloc(sizeof(Node));
        build_node(inner_cursor);
        Node temp_sprout = new_sprout(build_node(inner_cursor), modifier);
        memcpy(sprout, &temp_sprout, sizeof(Node));
        return sprout;
    }
    else {
        Node * left = build_node(left_expression_cursor(&cursor));
        Node * right = build_node(right_expression_cursor(&cursor));
        Combinator combinator = find_combinator(&cursor);
        NodeValue node_value;
        node_value.combinator = combinator;
        Node node = {FORK, node_value, left, right};
        Node * node_ptr = malloc(sizeof(Node));
        memcpy(node_ptr, &node, sizeof(Node));
        return node_ptr;
    }
}

Node build_tree(Token * tokens, size_t start_token, size_t end_token){
    Cursor cursor = new_cursor(tokens, start_token, end_token);
    return *build_node(cursor);
}




















