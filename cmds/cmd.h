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

// Holds Cmd and its redirections and arguments.
typedef struct Cmd {
    LList *redirections;
    char **args;
} Cmd;

// Creates an empty cmd.
Cmd *create_empty_cmd(int num_tokens);

// Deallocates the cmd.
void dealloc_cmd_specific(Cmd *cmd);

void dealloc_cmd(void **data);

void print_cmd(void *cmd);

#endif