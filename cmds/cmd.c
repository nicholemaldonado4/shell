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
Cmd *create_empty_cmd(int num_tokens) {
    Cmd *cmd = (Cmd *)malloc(sizeof(Cmd));
    if (cmd == NULL) {
        return NULL;
    }
    
    // Set command args to be MAX_NUM_TOKENS + 1 since args must
    // always end in NULL.
    cmd->args = (char **)malloc((num_tokens + 1) * sizeof(char *));
    if (cmd->args == NULL) {
        free(cmd);
        return NULL;
    }
    *(cmd->args) = NULL;
    
    // Each command can have a list of redirections.
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
    // Clean llist of redirections.
    if (cmd->redirections != NULL) {
        dealloc_ll(&(cmd->redirections), dealloc_redirection);
    }
    
    // Clean args.
    if (cmd->args != NULL) {
        int i = 0;
        while (cmd->args[i] != NULL) {
            free(cmd->args[i]);
            i++;
        }
        free(cmd->args);
    }
    
    // Clean cmd.
    free(cmd);
    cmd = NULL;
}

/*
 * Deallocates the cmd specified by *data.
 * This function is used when deallocating a LList
 * of commands.
 * Input: The cmd to deallocate.
 * Output: None.
 */
void dealloc_cmd(void **data) {
    dealloc_cmd_specific((Cmd *)(*data));
    *data = NULL;
}