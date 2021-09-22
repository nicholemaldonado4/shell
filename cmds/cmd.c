// Nichole Maldonado
// This file contains utility functions
// for a Cmd.

#include "cmd.h"

#include <stdio.h>
#include <stdlib.h>

#include "shell_consts.h"

/*
 * Creates an empty command.
 * Input: None.
 * Output: The allocated command.
 */
Cmd *create_empty_cmd() {
    Cmd *cmd = (Cmd *)malloc(sizeof(Cmd));
    if (cmd == NULL) {
        return NULL;
    }
    cmd->args = (char **)malloc((MAX_NUM_TOKENS + 1) * sizeof(char *));
    if (cmd->args == NULL) {
        free(cmd);
        return NULL;
    }
    *(cmd->args) = NULL;
    
    cmd->redirections = create_llist();
    if (cmd->redirections == NULL) {
        free(cmd->args);
        cmd->args = NULL;
        free(cmd);
        cmd = NULL;
    }
    return cmd;
}

/*
 * Deallocates the cmd.
 * Input: The cmd to deallocate.
 * Output: None.
 */
void dealloc_cmd_specific(Cmd *cmd) {
    if (cmd->redirections != NULL) {
        dealloc_ll(&(cmd->redirections), dealloc_redirection);
    }
    
    if (cmd->args != NULL) {
        int i = 0;
        while (cmd->args[i] != NULL) {
            free(cmd->args[i]);
            i++;
        }
        free(cmd->args);
    }
    free(cmd);
    cmd = NULL;
}