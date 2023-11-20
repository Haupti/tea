enum TokenType {
    IF,
    OPEN_CURLY,
    CLOSED_CURLY,
};

typedef struct Token {
    enum TokenType type;
    char * name;
} Token;
