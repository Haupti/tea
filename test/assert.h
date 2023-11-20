#ifndef ASSERT_HEADER
#define ASSERT_HEADER

#define DESCRIBE(x,y) int main(){\
    y\
    return 0;\
}

#define TEST(x,y) { \
    printf("    TEST "#x" ");\
    y\
};

#define ASSERT_EQUALS(x,y) {\
    if(x==y){\
        printf("SUCCESSFUL\n");\
    }\
    else {\
        printf("FAILED: expected "#x"=="#y", but was not.\n");\
    }\
}

#define ASSERT_STR_EQUALS(x,y) {\
    if(strcmp(x, y) == 0){\
        printf("SUCCESSFUL\n");\
    }\
    else {\
        printf("FAILED: expected %s==%s, but was not.\n", x,y);\
    }\
}

#endif
