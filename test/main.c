#include "assert.h"
#include <stdlib.h>
#include "evaluate_node_tests.h"
#include "build_tree_tests.h"
#include "syntax_verify_tests.h"
#include "read_tokens_tests.h"
#include "internal_functions_tests.h"
#include "sanity_check_tests.h"

int main(){
    RUN_DESCRIBE_MODULE(evaluate_node_tests)
    RUN_DESCRIBE_MODULE(build_tree_tests)
    RUN_DESCRIBE_MODULE(syntax_verify_tests)
    RUN_DESCRIBE_MODULE(read_tokens_tests)
    RUN_DESCRIBE_MODULE(internal_functions_tests)
    RUN_DESCRIBE_MODULE(sanity_check_tests)
    return EXIT_SUCCESS;
}
