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
    printf("    TEST "#x"\n");\
    y\
    if(failure_count == 0){\
        printf("SUCCESSFUL");\
    }\
};

#define ASSERT_EQUALS(x,y) {\
    assertion_count += 1;\
    if(x!=y){\
        failure_count += 1;\
        printf("        FAILURE in assertion %d: expected "#x"=="#y", but was not.\n", assertion_count);\
    }\
}

#define ASSERT_STR_EQUALS(x,y) {\
    assertion_count += 1;\
    if(strcmp(x, y) != 0){\
        failure_count += 1;\
        printf("        FAILURE in assertion %d: expected %s==%s, but was not.\n", assertion_count,x,y);\
    }\
}

#endif
