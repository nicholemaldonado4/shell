#include <stdio.h>
#include <stdlib.h>
#include "cmd.h"
#include "bool.h"
#include "llist.h"
#include "sub_cmd.h"
//#include "redirection.h"

// USED and .
// Creates an empty Cmd. An empty Cmd has one SubCmd allocated.
// The SubCmd's args will be NULL and its redirections will be
// and empty LList.
Cmd *create_empty_cmd() {
    Cmd *cmd = (Cmd *)malloc(sizeof(Cmd));
    if (cmd == NULL) {
        return cmd;
    }
    cmd->is_background = FALSE;
    cmd->sub_cmds = NULL;
    
    LList *sub_cmds = (LList *)malloc(sizeof(LList));
    if (sub_cmds == NULL) {
        free(cmd);
        return NULL;
    }
    sub_cmds->head = NULL;
    sub_cmds->tail = NULL;
    
    SubCmd *subcmd = create_empty_sub_cmd();
    if (subcmd == NULL) {
        free(cmd);
        free(sub_cmds);
        cmd = NULL;
        return cmd;
    }
    append(sub_cmds, subcmd);
    cmd->sub_cmds = sub_cmds;
    return cmd;
}

// USED and VERIFIED.
void dealloc_cmd_specific(Cmd *cmd) {
    if (cmd->sub_cmds != NULL) {
        dealloc_ll(&(cmd->sub_cmds), dealloc_sub_cmd);
    }
    free(cmd);
}

// USED and VERIFIED
void dealloc_cmd(void **data) {
    dealloc_cmd_specific((Cmd *)(*data));
    *data = NULL;
}

void dealloc_sub_2(LList *data) {
    Node *curr = (data)->head;
    while (curr != NULL) {
        Node *next = curr->next;
        SubCmd *sub_cmd = (SubCmd *)next->val;
        
        
        
        free(sub_cmd);
        free(curr);
        curr = next;
    }
    free(data);
}

// USED and VERIFIED.
void dealloc_cmds(LList **cmds) {
    dealloc_ll(cmds, dealloc_cmd);
}

// USED and VERIFIED.
void delete_last_cmd(LList **cmds) {
    if (cmds == NULL || *cmds == NULL) {
        return;
    }
    Node *curr_node = (*cmds)->head;
    while (curr_node != NULL && curr_node != (*cmds)->tail && curr_node->next != (*cmds)->tail) {
        curr_node = curr_node->next;
    }
    if (curr_node == NULL) {
        return;
    }
    bool has_one_cmd = (curr_node == (*cmds)->tail);
    dealloc_node((*cmds)->tail, dealloc_cmd);
    if (has_one_cmd) {
        (*cmds)->tail = NULL;
        (*cmds)->head = NULL;
        return;
    }
    curr_node->next = NULL;
    (*cmds)->tail = curr_node;  
}

// Adds the redirection to the cmds last SubCmd.
bool add_redir_to_last_sub(Cmd* cmd, Redirection *redir) {
    Node *node = cmd->sub_cmds->tail;
    SubCmd *last_sub = (SubCmd *)(node->val);

    if (!append(last_sub->redirections, redir)) {
        return FALSE;
    }
    return TRUE;
}

void set_args_for_last_sub(Cmd *cmd, char **args) {
    Node *node = cmd->sub_cmds->tail;
    SubCmd *last_sub = (SubCmd *)(node->val);
    last_sub->args = args;
}

void print_cmd(void *data) {
    Cmd *cmd = (Cmd *)data;
    printf("COMMAND:\n");
    printf("   is_background: %s\n", (cmd->is_background) ? "true" : "false");
    if (cmd->sub_cmds != NULL) {
        print_ll(cmd->sub_cmds, print_sub_cmd);
    }
    
}

bool has_single_sub_cmd(Cmd *cmd) {
    return cmd->sub_cmds != NULL && 
            cmd->sub_cmds->head != NULL && 
            cmd->sub_cmds->head == cmd->sub_cmds->tail;
}