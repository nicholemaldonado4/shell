#include <stdio.h>
#include <stdlib.h>
#include "redirection.h"

// USED and VERIFIED.
Redirection *create_redirection(char *name, RedirType redir_type) {
    Redirection *redir = (Redirection *)malloc(sizeof(Redirection));
    
    if (redir == NULL) {
        return redir;
    }
    redir->name = name;
    redir->type = redir_type;
    return redir;
}

void print_redir(void *redir) {
    Redirection *curr = (Redirection *)redir;
    if (curr != NULL) {
        char *type = (curr->type == INPUT) ? "input" : "output";
    
        printf("         Redirection: name: %s, type: %s\n", curr->name, type);
    }
    
}

void dealloc_redirection_specific(Redirection *redir) {
    if (redir->name != NULL) {
        free(redir->name);
        redir->name = NULL;
    }
    free(redir);
}

// USED and VERIFIED.
void dealloc_redirection(void **redir) {
    dealloc_redirection_specific((Redirection *)(*redir));
    *redir = NULL;
    
}

// USED and VERIFIED.
void dealloc_redirections(LList **redirs) {
    dealloc_ll(redirs, dealloc_redirection);
}