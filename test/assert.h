#ifndef ASSERT_HEADER
#define ASSERT_HEADER

#define DESCRIBE(x,y) int main(){\
    printf("DESCRIBE " #x ":\n");\
    y\
    return 0;\
}

#define TEST(x,y) { \
    int failure_count = 0;\
    int assertion_count = 0;\
    char test_name[100] = x;\
    y\
    if(failure_count == 0){\
        printf("    SUCCESSFUL TEST "#x"\n");\
    }\
};
#define XTEST(x,y) {printf("    SKIPPED TEST "#x"\n"); }

#define ASSERT_EQUALS(x,y) {\
    assertion_count += 1;\
    if(x!=y){\
        failure_count += 1;\
        printf("    FAILURE IN TEST \"%s\". failed in assertion %d: expected "#x"=="#y", but was not.\n", test_name, assertion_count);\
    }\
}

#define ASSERT_STR_EQUALS(x,y) {\
    assertion_count += 1;\
    if(strcmp(x, y) != 0){\
        failure_count += 1;\
        printf("    FAILURE IN TEST \"%s\". failed in assertion %d: expected %s==%s, but was not.\n", test_name, assertion_count,x,y);\
    }\
}

#endif
