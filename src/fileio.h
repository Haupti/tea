#ifndef FILEIO_HEADER
#define FILEIO_HEADER

typedef struct FileContent {
    char * buffer;
    long size;
} FileContent;

// buffer in file content must be free'd if not required anymore
FileContent read_file(char * filename);

#endif
