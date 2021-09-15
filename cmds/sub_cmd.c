#include <stdio.h>
#include <stdlib.h>

#include "sub_cmd.h"
//#include "bool.h"
//#include "redirection.h"

// USED and VERIFIED
// Creates an empty SubCmd. An empty SubCmd has NULL args and
// an empty LList of redirections.
SubCmd *create_empty_sub_cmd() {
    SubCmd *subcmd = (SubCmd *)malloc(sizeof(SubCmd));
    if (subcmd == NULL) {
        return NULL;
    }
    subcmd->args = NULL;
    subcmd->redirections = (LList *)malloc(sizeof(LList));
    subcmd->redirections->head = NULL;
    subcmd->redirections->tail = NULL;
    if (subcmd->redirections == NULL) {
        free(subcmd);
        subcmd = NULL;
    }
    return subcmd;
}

// USED and VERIFIED
void dealloc_sub_cmd_specific(SubCmd *sub_cmd) {
    if (sub_cmd->redirections != NULL) {
        dealloc_ll(&(sub_cmd->redirections), dealloc_redirection);
    }
    
    if (sub_cmd->args != NULL) {
        int i = 0;
        while (sub_cmd->args[i] != NULL) {
            free(sub_cmd->args[i]);
            i++;
        }
        free(sub_cmd->args);
    }
    free(sub_cmd);
    sub_cmd = NULL;
}

// USED and VERIFIED
void dealloc_sub_cmd(void **data) {
    dealloc_sub_cmd_specific((SubCmd *)(*data));
  
//    *data = NULL;
    
}

static void print_args(char **args) {
    int i = 0;
    printf("       args: {");
    while (args[i] != NULL) {
        printf("[%s]", args[i]);
        i++;
    }
    printf("}\n");
}

void print_sub_cmd(void *data) {
    
    SubCmd *sub_cmd = (SubCmd *)data;
    printf("    SUBCMD:\n");
    if (sub_cmd != NULL) {
        if (sub_cmd->args != NULL) {
            print_args(sub_cmd->args);
        }
        
        print_ll(sub_cmd->redirections, print_redir);
    }
}