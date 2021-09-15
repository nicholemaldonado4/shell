#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cmd_parser.h"
#include "llist.h"
#include "cmd.h"
#include "sub_cmd.h"
#include "exec_cmd.h"
#include "zombies.h"
#include "bool.h"

static const char *getenv_or_default(const char* val, const char *default_val);

// Variables
//static char *prompt_ending; = getenv_or_default("PS1", "");
static const int USER_INPUT_MAX_SIZE = 200;


static const char *getenv_or_default(const char* val, const char *default_val) {
    char *env_var = getenv(val);
    return env_var == NULL ? default_val : env_var;
}

static bool exec_cmds(char *cmds_str) {
//
//    Cmd *cmd = create_empty_cmd();
//
////    dealloc_cmd_specific(cmd);
//    LList *cmds = (LList *)malloc(sizeof(LList));
////    append(cmds, cmd);
//    Node *cmd_node = (Node *)malloc(sizeof(Node));
//    cmd_node->val = cmd;
//    cmd_node->next = NULL;
//    cmds->head = cmd_node;
//    cmds->tail = cmd_node;
//    dealloc_cmds(&cmds);
    
    LList *cmds = get_list_of_cmds(cmds_str);
    
    if (cmds == NULL) {
        printf(" No further commands will be executed.\n");
        return FALSE;
    }
    if (is_empty(cmds)) {
        dealloc_cmds(&cmds);
        return FALSE;
    }
    bool end_shell = execute(cmds);
//    printf("seg here\n");
    dealloc_cmds(&cmds);
//    printf("can't see this\n");
//    return end_shell;
    return end_shell;
}

static void run() {
    const char *prompt_ending = getenv_or_default("PS1", "$$$$");
    bool exit_shell = FALSE;
    
    char cwd[FILENAME_MAX];
    char user_input[USER_INPUT_MAX_SIZE];
    while (!exit_shell) {
        check_zombies();
        
        
        // TODO: getcwd can return NULL, so in this case make it blank if null.
        printf("%s%s ", getcwd(cwd, FILENAME_MAX * sizeof(char)), prompt_ending);
        if (fgets(user_input, USER_INPUT_MAX_SIZE, stdin) == NULL) {
            printf("\n");
            return;
        }
        exit_shell = exec_cmds(user_input);
        user_input[0] = '\0';
    }
}

int main() {
    run();
//    LList *cmds = get_list_of_cmds("echo hello>myFile.txt<a.txt| cat ok&|");
////    LList *cmds = get_list_of_cmds("e&|");
//    if (cmds == NULL) {
//        return 0;
//    }
//    
//    print_ll(cmds, print_cmd);
//    dealloc_ll(&cmds, dealloc_cmd);
}