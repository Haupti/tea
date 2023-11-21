#include <stdio.h>
#include <stdlib.h>

#include "cursor.h"
#include "token.h"

//[0,1,2,3,4,5]
//   ^   ^
//   s   e
//start = 1, end = 3;
// size = 3
int size(Cursor * it){
    return it->end-it->start+1;
}

Token next(Cursor * it){
    if(it->end < (it->position + 1)){
        puts("index out of bounds");
        exit(EXIT_FAILURE);
    }
    it->position += 1;
    return it->arr[(it->position)];
}

Token prev(Cursor * it){
    if(it->start < (it->position - 1)){
        puts("index out of bounds");
        exit(EXIT_FAILURE);
    }
    it->position -= 1;
    return it->arr[(it->position)];
}

Cursor copy_cursor(Cursor * original){
    Cursor new  = {original->arr, original->start, original->end, original->position};
    return new;
}

Token current(Cursor * it){
    if(it->end == 0){
        puts("iterator empty");
        exit(EXIT_FAILURE);
    }
    return it->arr[(it->position)];
}

Cursor new_cursor(Token * arr, size_t start, size_t end){
    if(start > end){
        puts("cursor start after cursor end");
        exit(EXIT_FAILURE);
    }
    Cursor it = {arr, start, end, start};
    return it;
}

int has_next(Cursor * it){
    return (it->position < (int) it->end);
}

int has_prev(Cursor * it){
    return it->position > (int) it->start;
}

void jump_to(Cursor * it, int pos){
    if(it->end < pos){
        puts("index out of bounds");
        exit(EXIT_FAILURE);
    }
    it->position = pos;
}

Token start(Cursor * it){
    it->position = it->start;
    return it->arr[it->position];
}
