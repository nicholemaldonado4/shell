// Nichole Maldonado
// This file is a header that defines 
// a Cmd. A Cmd have a variable number 
// of redirections. It can have at most
// MAX_NUM_TOKEN args.

#ifndef CMD_H
#define CMD_H

#include "bool.h"
#include "redirection.h"
#include "llist.h"

typedef struct Cmd {
    LList *redirections;
    char **args;
} Cmd;

// Creates an empty cmd.
Cmd *create_empty_cmd();

// Deallocates the cmd.
void dealloc_cmd_specific(Cmd *cmd);

#endif