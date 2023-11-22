#include "fileio.h"
#include <stdio.h>
#include <string.h>

int read_file(char * filename, char * target, int max_bytes){
    size_t size_tracker = 0;
    FILE * fptr;
    fptr = fopen(filename, "r");
    char line[200];
    while(fgets(line, 200, fptr) != NULL){
        if(size_tracker + strlen(line) > max_bytes){
            return -1;
        }
        size_tracker += strlen(line);
        strcat(target, line);
    }
    fclose(fptr);
    return 0;
}

int file_size(char * filename){
    FILE * fptr;
    fptr = fopen(filename, "r");
    fseek(fptr, 0, SEEK_END);
    int size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    fclose(fptr);
    return size;
}
