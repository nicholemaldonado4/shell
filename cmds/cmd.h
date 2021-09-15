#ifndef CMD_H
#define CMD_H

#include "bool.h"
#include "llist.h"
#include "redirection.h"


typedef struct Cmd {
    bool is_background;
    LList *sub_cmds;
} Cmd;

Cmd *create_empty_cmd();

void dealloc_cmd_specific(Cmd *cmd) ;

void dealloc_cmd(void **cmd);

void dealloc_cmds(LList **cmds);

void delete_last_cmd(LList **cmds);

bool add_redir_to_last_sub(Cmd *cmd, Redirection *redir);

void set_args_for_last_sub(Cmd *cmd, char **args);

void print_cmd(void *cmd);

bool has_single_sub_cmd(Cmd *cmd);

#endif