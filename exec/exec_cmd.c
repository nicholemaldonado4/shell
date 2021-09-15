#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "exec_cmd.h"
#include "llist.h"
#include "bool.h"
#include "sub_cmd.h"
#include "cmd.h"
#include "str_utils.h"
#include "zombies.h"
#include "exec_from_file.h"
#include "shell_consts.h"

// TODO: Check if it is ok that cd with no args does nothing.
// Otherwise I could use the $HOME var to change directory to the home?

// TODO: See if in execute we should allow "./" so if we want to run a prog in
// a current directory. Also maybe add functionality which will try to exec a
// prog in the current directory.

// TODO: is it ok if we just support < or >. Do we also have to support >> or << or 2>



static void execute_sub_cmds(LList *sub_cmds, bool is_background);

extern char **environ;


static void change_directory(char **args) {
    if (chdir(args[1]) < 0) {
        printf("No such file or directory exists.\n");
    }
}

// Executes the cmd. Returns true if curr_cmd is an exit cmd.
static bool deterimine_cmd_exec(LList *cmds, Cmd *curr_cmd) {
    if (!has_single_sub_cmd(curr_cmd)) {
        execute_sub_cmds(curr_cmd->sub_cmds, curr_cmd->is_background);
        return FALSE;
    }
    SubCmd *sub_cmd = (SubCmd *)curr_cmd->sub_cmds->head->val;
    if (strcmp(sub_cmd->args[0], EXEC_FROM_FILE) == 0) {
        LList *extra_cmds = exec_from_file(sub_cmd);
        if (extra_cmds != NULL) {
            append_ll(cmds, &extra_cmds);
        }
        return FALSE;
    } else if (strcmp(sub_cmd->args[0], CD_CMD) == 0) {
        change_directory(sub_cmd->args);
        return FALSE;
    } else if (strcmp(sub_cmd->args[0], EXIT_CMD) == 0) {
        return TRUE;
    }
    execute_sub_cmds(curr_cmd->sub_cmds, curr_cmd->is_background);
    return FALSE;
}

bool execute(LList *cmds) {
    Node *curr_cmd_node = cmds->head;
    while (curr_cmd_node != NULL) {
        Cmd *cmd = (Cmd *)curr_cmd_node->val;
        if (deterimine_cmd_exec(cmds, cmd)) {
            return TRUE;
        }
        curr_cmd_node = curr_cmd_node->next;
    }
    return FALSE;
}

//static void dealloc_fd_tups(FdTuple **fd_tup, int size) {
//    for (int i = 0; i < size; i++) {
//        close(fd_tup[i]->pw);
//        close(fd_tup[i]->pr);
//        free(fd_tup[i]);
//        fd_tup[i] = NULL;
//    }
//    free(fd_tup);
//}
//
//static FdTuple **setup_fd_tups(int size) {
//    FdTuple **fd_tups = create_fd_tups(size - 1);
//    if (fd_tups == NULL) {
//        printf("ERROR: Unable to allocate memory.\n");
//        return fd_tups;
//    }
//    int pipes[2];
//    for (int i = 0; i < size - 1; i++) {
//        if (pipe(pipes) < 0) {
//            printf("ERROR: Unable to create pipe.\n");
//            dealloc_fd_tups(fd_tups, i);
//            return NULL;
//        }
//        fd_tups[i] = create_fd_tuple(pipes[0], pipes[1]);
//        if (fd_tups[i] == NULL) {
//            printf("ERROR: Unable to allocate memory.\n");
//            dealloc_fd_tups(fd_tups, i);
//            return NULL;
//        }
//    }
//    return fd_tups;
//}

static char *str_copy_and_append(char *str, int start, int end, char *append_str, int append_str_len) {
    char *new_str = (char *)malloc((end - start + append_str_len + 2) * sizeof(char));
    if (new_str == NULL) {
        return NULL;
    }
    int sub_str_len = end - start;
    for (int i = 0; i < sub_str_len; i++) {
        new_str[i] = str[i + start];
    }
    new_str[sub_str_len] = '/';
    for (int i = 0; i < append_str_len; i++) {
        new_str[i + sub_str_len + 1] = append_str[i];
    }
    new_str[sub_str_len + append_str_len + 1] = '\0';
    return new_str;
}

static LList *get_possible_progs(char *prog_name) {
    LList *progs = (LList *)malloc(sizeof(LList));
    progs->head = NULL;
    progs->tail = NULL;
    char *paths = getenv("PATH");
    if (paths == NULL) {
        char *new_prog = strdup(prog_name);
        if (new_prog == NULL) {
            printf("Unable to allocate memory.\n");
            return NULL;
        }
        append(progs, new_prog);
        return progs;
    }
    int prog_len = strlen(prog_name);
    
    int i = 0;
    int path_start = 0;
    while (paths[i] != '\0') {
        if (paths[i] == ':') {
            char *new_prog = str_copy_and_append(paths, path_start, i, prog_name, prog_len);
            if (new_prog == NULL) {
                dealloc_ll(&progs, dealloc_str);
                return NULL;
            }
            append(progs, new_prog);
            path_start = i + 1;
        }
        i++;
    }
    return progs;
}

static void execute_program(char *program, char **args) {
//    printf("Prog: %s\n", program);
    execve(program, args, environ);
}

static void execute_single_sub_cmd(SubCmd *sub_cmd) {
    if (**(sub_cmd->args) == '/') {
        execute_program(*(sub_cmd->args), sub_cmd->args);
    } else {
        LList *progs = get_possible_progs((sub_cmd->args)[0]);
        if (progs == NULL) {
            exit(1);
        }
        
        Node *curr_prog_node = progs->head;
        while (curr_prog_node != NULL) {
            execute_program((char *)(curr_prog_node->val), sub_cmd->args);
            curr_prog_node = curr_prog_node->next;
        }
    }
    printf("Error: Unable to execute program.\n");
    exit(1);
}

static void setup_redirections(LList *redirections) {
    Node *redir_node = redirections->head;
    while (redir_node != NULL) {
        Redirection *redir = (Redirection *)redir_node->val;
        if (redir->name == NULL) {
            continue;
        }

        if (redir->type == INPUT) {
            close(STDIN_FILENO);
            int fd = open(redir->name, O_RDONLY);
            if (fd < 0) {
                printf("%s: no such file or directory exists\n", redir->name);
                exit(1);
            }
            
        } else {
            close(STDOUT_FILENO);
            int fd = open(redir->name, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            if (fd < 0) {
                printf("%s: no such file or directory exists\n", redir->name);
                exit(1);
            }
        }
        redir_node = redir_node->next;
    }
}

static void execute_sub_cmds(LList *sub_cmds, bool is_background) {
    if (sub_cmds->head == NULL) {
        return;
    }
    
    int num_sub_cmds = size_ll(sub_cmds);
    int fd[2];
    int *rcs = (int *)malloc(num_sub_cmds * sizeof(int));
    int prev_input = STDIN_FILENO;

    Node *curr_sub_node = sub_cmds->head;
    for (int curr_cmd_num = 0; curr_cmd_num < num_sub_cmds; curr_cmd_num++) {
        if (curr_cmd_num < num_sub_cmds - 1 && pipe(fd) < 0) {
            printf("ERROR: Unable to create pipe.\n");
            if (prev_input != STDIN_FILENO) {
                close(prev_input);
            }
            return;
        }
        int rc = fork();
        if (rc < 0) {
            if (prev_input != STDIN_FILENO) {
                close(prev_input);
            }
            if (curr_cmd_num < num_sub_cmds - 1) {
                close(fd[0]);
                close(fd[1]);
            }
            return;
        } else if (rc == 0) {
            // Set input (read).
            if (curr_cmd_num < num_sub_cmds - 1) {
                dup2(fd[1], STDOUT_FILENO);
            }
            if (curr_cmd_num > 0) {
                dup2(prev_input, STDIN_FILENO);
            }
            if (curr_cmd_num < num_sub_cmds - 1) {
                close(fd[0]);
                close(fd[1]);
            }
            if (curr_cmd_num > 0) {
                close(prev_input);
            }
            
            SubCmd *sub_cmd = (SubCmd *)curr_sub_node->val;
            if (strcmp(sub_cmd->args[0], EXEC_FROM_FILE) == 0 ||
               strcmp(sub_cmd->args[0], CD_CMD) == 0 ||
               strcmp(sub_cmd->args[0], EXIT_CMD) == 0) {
                exit(0);
            } else {
                setup_redirections(sub_cmd->redirections);
                execute_single_sub_cmd(sub_cmd);
            }
        } else {
            if (prev_input != STDIN_FILENO) {
                close(prev_input);
            }
            if (curr_cmd_num < num_sub_cmds - 1) {
                prev_input = fd[0];
                close(fd[1]);
            }
            if (is_background) {
                printf("[PID] %d\n", rc);
            }
            rcs[curr_cmd_num] = rc;
        }
        curr_sub_node = curr_sub_node->next;
    }

    if (!is_background) {
        for (int i = num_sub_cmds - 1; i >= 0; i--) {
            waitpid(rcs[num_sub_cmds - 1 - i], NULL, 0);
        }
    } else {
        check_zombies();
    }
    free(rcs);
    rcs = NULL;
}

//bool execute(LList *cmds) {
//    Node *curr_cmd_node = cmds->head;
//    while (curr_cmd_node != NULL) {
//        Cmd *cmd = (Cmd *)curr_cmd_node->val;
//        if (!is_shell_cmd(cmds, cmd)) {
//            execute_sub_cmds(cmd->sub_cmds, cmd->is_background);
//        }
////        SubCmd *sub_cmd = (SubCmd *)cmd->sub_cmds->head->val;
////        if (strcmp(sub_cmd->args[0], EXEC_FROM_FILE) == 0) {
////            LList *extra_cmds = exec_from_file(sub_cmd);
////            if (extra_cmds != NULL) {
////                append_ll(cmds, &extra_cmds);
////            }
////        } else {
////            execute_sub_cmds(cmd->sub_cmds, cmd->is_background);
////        }
//        
//        curr_cmd_node = curr_cmd_node->next;
//    }
//    return FALSE;
//}

//static int execute_last_sub(int prev_input, SubCmd *sub_cmd, bool is_background) {
//    int rc = fork();
//    if (rc < 0) {
//        printf("fork failed");
//        return -1;
//    } else if (rc == 0) {
//        // Set input (read).
//        dup2(prev_input, STDIN_FILENO);
//        if (prev_input != STDIN_FILENO) {
//            close(prev_input);
//        }
//        
//        if (strcmp(sub_cmd->args[0], EXEC_FROM_FILE) == 0) {
//            exit(0);
//        }
//
//        execute_single_sub_cmd(sub_cmd);
//    } else {
//        if (is_background) {
//                printf("[PID] %d\n", rc);
//        }
//    }
//    return rc;
//}


//static void execute_sub_cmds(LList *sub_cmds, bool is_background) {
//    if (sub_cmds->head == NULL) {
//        return;
//    }
//    
//    int num_sub_cmds = size_ll(sub_cmds);
//    int fd[2];
//    int *rcs = (int *)malloc(num_sub_cmds * sizeof(int));
//    int prev_input = STDIN_FILENO;
//    int curr_cmd_num = 0;
//    
//    Node *curr_sub_node = sub_cmds->head;
//    while (curr_sub_node->next != NULL) {
//        if (pipe(fd) < 0) {
//            printf("ERROR: Unable to create pipe.\n");
//            if (prev_input != STDIN_FILENO) {
//                close(prev_input);
//            }
//            return;
//        }
//        
//        int rc = fork();
//        if (rc < 0) {
//            printf("fork failed\n");
//            if (prev_input != STDIN_FILENO) {
//                close(prev_input);
//            }
//            close(fd[0]);
//            close(fd[1]);
//            return;
//        } else if (rc == 0) {
//            // Set input (read).
//            dup2(prev_input, STDIN_FILENO);
//            
//            // Set output (write).
//            dup2(fd[1], STDOUT_FILENO);
//            
//            close(fd[0]);
//            close(fd[1]);
//            if (prev_input != STDIN_FILENO) {
//                close(prev_input);
//            }
//            SubCmd *sub_cmd = (SubCmd *)curr_sub_node->val;
//            if (strcmp(sub_cmd->args[0], EXEC_FROM_FILE) == 0) {
//                exit(0);
//            } else {
//                execute_single_sub_cmd(sub_cmd);
//            }
//        } else {
//            if (prev_input != STDIN_FILENO) {
//                close(prev_input);
//            }
//            prev_input = fd[0];
//            close(fd[1]);
//            if (is_background) {
//                printf("[PID] %d\n", rc);
//            }
//            rcs[curr_cmd_num++] = rc;
//        }
//        curr_sub_node = curr_sub_node->next;
//    }
//    int last_rc = execute_last_sub(prev_input, (SubCmd *)curr_sub_node->val, is_background);
//    rcs[curr_cmd_num] = last_rc;
//    if (prev_input != STDIN_FILENO) {
//        close(prev_input);
//    }
//    if (last_rc > 0) {
//        if (!is_background) {
//            for (int i = num_sub_cmds - 1; i >= 0; i--) {
//                waitpid(rcs[num_sub_cmds - 1 - i], NULL, 0);
//            }
//        } else {
//            check_zombies();
//        }
//    }
//    free(rcs);
//    rcs = NULL;
//}





//static void execute_sub_cmds(LList *sub_cmds, bool is_background) {
//    int num_sub_cmds = size_ll(sub_cmds);
//    FdTuple **fd_tups = setup_fd_tups(num_sub_cmds);
//    if (fd_tups == NULL) {
//        return;
//    }
//    int last_rc = 0;
//    
//    Node *curr_sub_node = sub_cmds->head;
//    int sub_num = 0;
//    while (curr_sub_node != NULL && sub_num < num_sub_cmds) {
//        int rc = fork();
//        
//        if (rc < 0) {
//            printf("fork failed\n");
//            exit(1);
//        } else if (rc == 0) {
//            
//            // Write to pipe.
//            if (sub_num < num_sub_cmds - 1) {
//                dup2(fd_tups[sub_num]->pw, STDOUT_FILENO);
//                
//            }
//            if (sub_num > 0) {
//                dup2(fd_tups[sub_num - 1]->pr, STDIN_FILENO);
//            }
//            dealloc_fd_tups(fd_tups, num_sub_cmds - 1);
//            execute_single_sub_cmd((SubCmd *)curr_sub_node->val);
//        } else {
//            last_rc = rc;
//        }
//        sub_num++;
//        curr_sub_node = curr_sub_node->next;
//    }
//    dealloc_fd_tups(fd_tups, num_sub_cmds - 1);
//    waitpid(last_rc, NULL, 0);
//    
//}