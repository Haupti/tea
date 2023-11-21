#ifndef LEXER_HEADER
#define LEXER_HEADER

#include <stdlib.h>
#include "token.h"

// reads tokens and saves them to storage
// returns number of tokens read
// throws on failure while reading tokens
int read_tokens(Token * tokens, char * input, size_t input_len);

#endif
