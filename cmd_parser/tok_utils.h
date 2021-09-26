// Nichole Maldonado
// This file stores defines TokenizeUtils
// used when tokenizing commands.

#ifndef TOK_UTILS_H
#define TOK_UTILS_H

// Struct used to help when tokenizing commands.
// total_tokens is the total number of tokens that
// have been seen. line is the line of user input.
// token is a buffer created to store the current
// token being made.
typedef struct TokenizeUtils {
    int total_tokens;
    char *line;
    char *token;
    
} TokenizeUtils;

// Creates TokenizeUtils.
TokenizeUtils *create_tok_utils(char *line);

// Deallocates TokenizeUtils. Does not deallocate line.
void dealloc_tok_utils(TokenizeUtils **tok_utils);

#endif