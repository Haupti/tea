#include "slice.h"
#include "err.h"

Slice new_slice(Token * arr, int start, int end){
    Slice slice = { arr, start ,end};
    return slice;
}

Token token_at(Slice slice, int index){
    if(index > slice.end || index < slice.start){
        err("index out of bounds");
    }
    return slice.arr[index];
}

Token last_token(Slice slice){
    return slice.arr[slice.end];
}
