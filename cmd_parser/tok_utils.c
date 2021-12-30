// Nichole Maldonado
// This file stores utils used
// to tokenize commands.

#include "tok_utils.h"

#include <stdio.h>
#include <stdlib.h>

#include "shell_consts.h"

/*
 * Initializes TokenizeUtils.
 * Input: line that will be assigned to TokenizeUtils'
 *        line. It will not be copied.
 * Output: The TokenizeUtils with line set and total_tokens set to 0.
 *         Null on error.
 */
TokenizeUtils *create_tok_utils(char *line) {
    TokenizeUtils *tok_utils = (TokenizeUtils *)malloc(sizeof(TokenizeUtils));
    if (tok_utils == NULL) {
        return NULL;
    }
    
    tok_utils->token = (char *)malloc((MAX_TOKEN_SIZE + 1) * sizeof(char));
    if (tok_utils->token == NULL) {
        free(tok_utils);
        return NULL;
    }
    
    tok_utils->line = line;
    tok_utils->total_tokens = 0;
    return tok_utils;
}

/*
 * Deallocates TokenizeUtils.
 * Input: the TokenizeUtils to deallocatee.
 * Output: NONE.
 */
void dealloc_tok_utils(TokenizeUtils **tok_utils) {
    if ((*tok_utils)->token != NULL) {
        free((*tok_utils)->token);
        (*tok_utils)->token = NULL;
    }
    
    // Intentionally do not deallocate line since
    // we did not malloc data for line.
    (*tok_utils)->line = NULL;
    free(*tok_utils);
    *tok_utils = NULL;
}