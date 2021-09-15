#ifndef SUBCMD_H
#define SUBCMD_H

#include "bool.h"
#include "redirection.h"
#include "llist.h"

typedef struct SubCmd {
    LList *redirections;
    char **args;
} SubCmd;

SubCmd *create_empty_sub_cmd();

void dealloc_sub_cmd_specific(SubCmd *sub_cmd);

void dealloc_sub_cmd(void **sub_cmd);

void print_sub_cmd(void *sub_cmd);

#endif