// Nichole Maldonado
// This file takes a line and splits it by  
// whitespace into a an array of tokens.

#include  <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell_consts.h"
#include "bool.h"
#include "str_utils.h"

/*
 * Initializes tokens as an array that can hold
 * MAX_NUM_TOKENS + 1 character pointers (The last
 * should be NULL). Initializes token to hold
 * MAX_TOKEN_SIZE + 1 characters.
 * Input: tokens and token which will be initialized with
 *        malloc data.
 * Output: TRUE if all the data was able to be allocated
 *         FALSE otherwise.
 */
static bool init_toks(char ***tokens, char **token) {
    *tokens = (char **)malloc((MAX_NUM_TOKENS + 1) * sizeof(char *));    
    if (*tokens == NULL) {
        printf("Shell: Unable to allocate memory.\n");
        return FALSE;
    }

    *token = (char *)malloc((MAX_TOKEN_SIZE + 1) * sizeof(char));
    if (*token == NULL) {
        printf("Shell: Unable to allocate memory.\n");
        free(*tokens);
        *tokens = NULL;
        return FALSE;
    }
    return TRUE;
}

/*
 * Deallocates tokens and token.
 * Input: tokens which is a malloced char ** and token
 *        which is a malloced char *. token_num - 1 is
 *        the last char * that was malloced and stored in
 *        tokens.
 * Output: None.
 */
static void dealloc_toks(char ***tokens, char **token, int token_num) {
    (*tokens)[token_num] = NULL;
    dealloc_str_arr(tokens);
    free(*token);
    *token = NULL;
}

/*
 * Splits the line by whitespace and returns the 
 * corresponding array of tokens.
 * Input: A line of tokens seperated by whitespace.
 * Output: None.
 */
char **tokenize(char *line) {
    char **tokens = NULL;
    char *token = NULL;
    if (!init_toks(&tokens, &token)) {
        return NULL;
    }
    
    int i = 0;
    int token_index = 0;
    int token_num = 0;
    while (line[i] != '\0'){
        char curr_char = line[i];
        if (curr_char == ' ' || curr_char == '\n' || curr_char == '\t') {
            token[token_index] = '\0';
            
            // If we currently have tokens to store, store
            // them.
            if (token_index != 0) {
                if (token_num >= MAX_NUM_TOKENS) {
                    printf("Shell: Token number exceeded max size limit.\n");
                    dealloc_toks(&tokens, &token, token_num);
                    return NULL;
                }
                
                tokens[token_num] = strdup(token);
                if (tokens == NULL) {
                    printf("Shell: Unable to allocate memory.\n");
                    dealloc_toks(&tokens, &token, token_num);
                    return NULL;
                }
                token_num++;
                token_index = 0; 
            }
        } else {
            if (token_index >= MAX_TOKEN_SIZE) {
                printf("Shell: Token was exceeded max size limit.\n");
                dealloc_toks(&tokens, &token, token_num);
                return NULL;
            }
            token[token_index++] = curr_char;
        }
        i++;
    }
 
    free(token);
    tokens[token_num] = NULL;
    return tokens;
}