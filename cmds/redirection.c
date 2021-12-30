// Nichole Maldonado
// This file defines a utility functions
// for the Redirection struct.

#include "redirection.h"

#include <stdio.h>
#include <stdlib.h>

// Deallocates the redirection.
static void dealloc_redirection_specific(Redirection *redir);

/*
 * Creates a redirection.
 * Input: The file name and redirection type.
 * Output: A Redirection.
 */
Redirection *create_redirection(char *name, RedirType redir_type) {
    Redirection *redir = (Redirection *)malloc(sizeof(Redirection));
    
    if (redir == NULL) {
        return redir;
    }
    redir->name = name;
    redir->type = redir_type;
    return redir;
}

/*
 * Deallocates the redirection.
 * Input: The redirection to deallocate.
 * Output: None.
 */
static void dealloc_redirection_specific(Redirection *redir) {
    if (redir->name != NULL) {
        free(redir->name);
        redir->name = NULL;
    }
    free(redir);
}

/*
 * Deallocates the redirections.
 * Input: a void** which actually refers to a redirection.
 * Output: None.
 */
void dealloc_redirection(void **redir) {
    dealloc_redirection_specific((Redirection *)(*redir));
    *redir = NULL; 
}

/*
 * Determines the redirection type.
 * Input: The curr character to determine if input or output.
 * Output: The redirection type.
 */
RedirType get_redir_type(char curr_char) {
    return curr_char == '>' ? OUTPUT : INPUT;
}