#ifndef ASSERT_HEADER
#define ASSERT_HEADER

#define COLOR_RED "\033[0;31m"
#define COLOR_CYAN "\033[0;36m"
#define COLOR_GREEN "\033[0;32m"
#define COLOR_RESET "\033[0m"

#define RUN_DESCRIBE_MODULE(describeFunctionName) describeFunctionName();

#define MODULAR_DESCRIBE_H(describeFunctionName) int describeFunctionName();

#define MODULAR_DESCRIBE(describeFunctionName,y) int describeFunctionName(){\
    printf(COLOR_GREEN);\
    printf("DESCRIBE " #describeFunctionName ":\n");\
    printf(COLOR_RESET);\
    int tests_failure_count = 0;\
    y\
    return tests_failure_count;\
}

#define DESCRIBE(x,y) int main(){\
    printf(COLOR_GREEN);\
    printf("DESCRIBE " #x ":\n");\
    printf(COLOR_RESET);\
    int tests_failure_count = 0;\
    y\
    return 0;\
}

#define TEST(x,y) { \
    int failure_count = 0;\
    int assertion_count = 0;\
    char test_name[100] = x;\
    y\
    if(failure_count == 0){\
        printf(COLOR_CYAN);\
        printf("    SUCCESSFUL TEST "#x"\n");\
        printf(COLOR_RESET);\
    }\
    else {\
        tests_failure_count +=1;\
    } \
};

#define XTEST(x,y) {printf("    SKIPPED TEST "#x"\n"); }

#define ASSERT_INT_EQUALS(x,y) {\
    assertion_count += 1;\
    if(x!=y){\
        failure_count += 1;\
        printf(COLOR_RED);\
        printf("    FAILURE IN TEST \"%s\". failed in assertion %d: expected %d==%d, but was not.\n", test_name, assertion_count, x, y);\
        printf(COLOR_RESET);\
    }\
}

#define ASSERT_EQUALS(x,y) {\
    assertion_count += 1;\
    if(x!=y){\
        failure_count += 1;\
        printf(COLOR_RED);\
        printf("    FAILURE IN TEST \"%s\". failed in assertion %d: expected "#x"=="#y", but was not.\n", test_name, assertion_count);\
        printf(COLOR_RESET);\
    }\
}

#define ASSERT_STR_EQUALS(x,y) {\
    assertion_count += 1;\
    if(strcmp(x, y) != 0){\
        failure_count += 1;\
        printf(COLOR_RED);\
        printf("    FAILURE IN TEST \"%s\". failed in assertion %d: expected %s==%s, but was not.\n", test_name, assertion_count,x,y);\
        printf(COLOR_RESET);\
    }\
}

#endif
