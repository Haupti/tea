#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "ast_node.h"
#include "cursor.h"
#include "err.h"


// p stands for pedantic
void * p_realloc(void * ptr, size_t size){
    void * new = realloc(ptr, size);
    if(new == NULL){
        puts("ERROR: out of continuous memory");
        exit(EXIT_FAILURE);
    }
    return new;
}



Value evaluate(Value v1, Combinator combinator, Value v2){
    switch(combinator){
        case COMBINATOR_AND:
            if(v1 == VALUE_ON && v2 == VALUE_ON){
                return VALUE_ON;
            }
            else{
                return VALUE_OFF;
            }
            break;
        case COMBINATOR_OR:
            if(v1 == VALUE_OFF && v2 == VALUE_ON){
                return VALUE_OFF;
            }
            else {
                return VALUE_ON;
            }
            break;
    }
}

// expects the cursor to be at the position of grp_close
// returns value between start and end: the position of the matching grp_close
// error if no matching grp_close could be found
int find_matching_grp_close(Cursor * cursor){
    if(current(cursor).type != GRP_OPEN){
        err("while searching bracket: cannot find matching bracket if im not at a bracket");
    }
    Cursor temp = copy_cursor(cursor);
    Token token = current(&temp);
    int bracket_counter = 1;
    while(has_prev(&temp)){
        token = prev(&temp);
        switch(token.type){
            case GRP_CLOSE:
                bracket_counter += 1;
                break;
            case GRP_OPEN:
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

// expects the cursor to be at the position of grp_open
// returns value between start and end: the position of the matching grp_open
// error if no matching grp_open could be found
int find_matching_grp_open(Cursor * cursor){
    if(current(cursor).type != GRP_CLOSE){
        err("while searching bracket: cannot find matching bracket if im not at a bracket");
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
    err("while searching bracket: no matching '(' found");
    return -1;
}

int is_value(Token token){
    return token.type == ON || token.type == OFF;
}

int is_combinator(Token token){
    return token.type == AND || token.type == OR;
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

Cursor left_expression_cursor(Cursor * cursor){
    if(is_value(current(cursor))){
        Cursor new_cursor = {cursor->arr, cursor->position, cursor->position, cursor->position};
        return new_cursor;
    }
    int start = cursor->position;
    int end = find_matching_grp_close(cursor);
    Cursor new_cursor = {cursor->arr, start, end, start};
    return new_cursor;
}

Cursor right_expression_cursor(Cursor * cursor){

    if(is_value(current(cursor))){
        next(cursor); // at combinator
        next(cursor); // at right open bracket
    }else{
        int left_close = find_matching_grp_close(cursor);
        jump_to(cursor, left_close);
        next(cursor); // at combinator
        next(cursor); // at right open bracket
    }
    int open_pos = cursor->position;
    int close_pos = find_matching_grp_close(cursor);
    Cursor new_cursor = {cursor->arr, open_pos, close_pos, open_pos};
    return new_cursor;
}

Combinator find_combinator(Cursor * cursor){
    Token token = current(cursor);
    if(is_combinator(token)){
        return to_combinator(token);
    }
    if(is_value(token)){
        if(!has_next(cursor)){
            err("unexpected end: expected a combinator");
        }
        return to_combinator(next(cursor));
    }
    int close_pos = find_matching_grp_close(cursor);
    jump_to(cursor, close_pos);
    token = next(cursor);
    return to_combinator(token);
}


Node * build_node(Cursor cursor){
    if(size(&cursor) == 1){
        Node * leaf = malloc(sizeof(Node));
        Node temp_leaf = new_leaf(to_value(current(&cursor)));
        memcpy(leaf, &temp_leaf, sizeof(Node));
        return leaf;
    }
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

Node build_tree(Token * tokens, size_t start_token, size_t end_token){
    Cursor cursor = new_cursor(tokens, start_token, end_token);
    return *build_node(cursor);
}




















