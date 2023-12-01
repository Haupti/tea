#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/ast_node.h"
#include "src/fileio.h"
#include "src/lexer.h"
#include "src/ast_builder.h"
#include "src/ast_evaluate.h"
#include "src/sanity_check.h"
#include "src/syntax_verify.h"
#include "src/token.h"
#include "src/err.h"
#include "src/tea.h"

int main(int args, char * argv[]){

    if(args != 2){
        puts("expected exaclty one argument (filename)");
        return EXIT_FAILURE;
    }

    // read file
    FileContent file_content = read_file(argv[1]);
    long filesize = file_content.size;
    char * program = file_content.buffer;


    Value val = tea(program, filesize);
    switch(val){
        case VALUE_ON:
            puts("1");
            break;
        case VALUE_OFF:
            puts("0");
            break;
    }
    return EXIT_SUCCESS;
}

