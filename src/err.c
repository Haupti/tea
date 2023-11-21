#include <stdio.h>
#include <stdlib.h>

void err(char * msg){
    printf("ERROR while evaluation: %s\n", msg);
    exit(EXIT_FAILURE);
}
