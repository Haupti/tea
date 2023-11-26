#include <stdio.h>
#include <stdlib.h>

void err(char * msg){
    printf("ERROR %s\n", msg);
    exit(EXIT_FAILURE);
}

void err_at(char * msg, int index){
    printf("ERROR (at %d) %s\n", index, msg);
    exit(EXIT_FAILURE);
}
