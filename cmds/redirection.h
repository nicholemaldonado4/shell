#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "llist.h"

typedef enum  {
    INPUT,
    OUTPUT
} RedirType;

typedef struct Redirection {
    char *name;
    RedirType type;
    
} Redirection;

Redirection *create_redirection(char *name, RedirType redir_type);

void print_redir(void *redir);

void dealloc_redirection_specific(Redirection *redirection);

void dealloc_redirection(void **redir);

void dealloc_redirections(LList **redirs);

#endif