// Nichole Maldonado
// This header defines a Redirection struct.

#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "llist.h"

typedef enum  {
    INPUT,
    OUTPUT
} RedirType;

// Holds a redirection with its filename and type.
typedef struct Redirection {
    char *name;
    RedirType type;
    
} Redirection;

// Creates a redirection.
Redirection *create_redirection(char *name, RedirType redir_type);

// Deallocates the redirection.
void dealloc_redirection(void **redir);

// Gets the redirection type.
RedirType get_redir_type(char curr_char);

#endif