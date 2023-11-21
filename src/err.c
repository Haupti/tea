#include <stdio.h>
#include <stdlib.h>

void err(char * msg){
    printf("ERROR %s\n", msg);
    exit(EXIT_FAILURE);
}
