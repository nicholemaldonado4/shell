// Nichole Maldonado
// This file forks() a child
// to execute the provided command.

#include "exec_cmd.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err_msg.h"
#include "llist.h"
#include "redirection.h"
#include "shell_consts.h"
#include "str_utils.h"

// Changes directory.
static void change_directory(Cmd *cmd);

// Runs the executable based on args.
static void run_executable(char **args);

// Sets up redirections for the current process.
static bool setup_redirs(LList *redirs, bool set_err_only);

// Forks child to run executable.
static void execute_cmd(Cmd *cmd);

/*
 * Changes the directory to the dir provided in args.
 * Input: args whose first char * is "cd". The second
 *        char * should be the directory. If more args
 *        are provided, they are ignored like the unix
 *        "cd" command.
 * Output: None.
 */
static void change_directory(Cmd *cmd) {
    int old_err_fd = dup(STDERR_FILENO);
    if (!setup_redirs(cmd->redirections, TRUE)) {
        dup2(old_err_fd, STDERR_FILENO);
        close(old_err_fd);
        return;
    }
    
    
    if (cmd->args[1] == NULL) {
//        printf("Shell: Incorrect command to display and prompting for the next command.\n");
        print_err();
        dup2(old_err_fd, STDERR_FILENO);
        close(old_err_fd);
        return;
    }
    
    if (chdir(cmd->args[1]) < 0) {
//        printf("Shell: cd: %s: No such file or directory\n", args[1]);
        print_err();
        dup2(old_err_fd, STDERR_FILENO);
        close(old_err_fd);
        return;
    }
    dup2(old_err_fd, STDERR_FILENO);
    close(old_err_fd);
    return;
}

/*
 * Runs the executable and exits() if unsuccessful.
 * Input: the char ** of args for the command.
 * Output: Null.
 */
static void run_executable(char **args) {
    execvp(args[0], args);
//    printf("Shell: %s: command not found\n", args[0]);
    print_err();
    exit(1);
}

/*
 * Setups all redirs.
 * Input: redirs which is a LList of redirections. If set_error_only
 *        is set to true, then only the stderror will be setup. 
 *        Otherwise, all file descriptors are setup.
 * Output: Null.
 */
static bool setup_redirs(LList *redirs, bool set_err_only) {
    Node *redir_node = redirs->head;
    while (redir_node != NULL) {
        Redirection *redir = (Redirection *)redir_node->val;
        if (redir->type == OUTPUT) {
            int fd = open(redir->name, O_CREAT | O_TRUNC | O_WRONLY,  S_IRUSR | S_IWUSR);
            if (fd < 0) {
                print_err();
                return FALSE;
            }
            if (!set_err_only) {
                dup2(fd, STDOUT_FILENO);
            }
            
            dup2(fd, STDERR_FILENO);
            close(fd);
        } else {
            int fd = open(redir->name, O_RDONLY);
            if (fd < 0) {
                print_err();
                return FALSE;
            }
            if (!set_err_only) {
                dup2(fd, STDIN_FILENO);
            }
            close(fd);
        }
        redir_node = redir_node->next;
    }
    return TRUE;
}

/*
 * Execute the command in args[0]. Forks
 * a child process to exeucte the command. The
 * parent waits for the child to finish.
 * Input: args which holds the command name
 *        followed by the arguments.
 * Output: Null.
 */
static void execute_cmd(Cmd *cmd) {
    int rc = fork();
    if (rc < 0) {
//        printf("Shell: Unable to fork.\n");
        print_err();
        return;
    } else if (rc == 0) {
        if (!setup_redirs(cmd->redirections, FALSE)) {
            exit(1);
        }
        run_executable(cmd->args);
    } else {
        wait(NULL);
    }
    
    return;
}

/*
 * Execute the command in args[0]. Forks
 * a child process to execute the command. The
 * parent waits for the child to finish.
 * Input: args which holds the command name
 *        followed by the arguments.
 * Output: Null.
 */
bool execute(Cmd *cmd) {

    // If we only have redirection, then basically open/close 
    // the input/output files.
    if (cmd->args[0] == NULL) {
        int old_err_fd = dup(STDERR_FILENO);
        setup_redirs(cmd->redirections, TRUE);
        dup2(old_err_fd, STDERR_FILENO);
    } else if (strcmp(cmd->args[0], CD_CMD) == 0) {
        change_directory(cmd);
    } else if (strcmp(cmd->args[0], EXIT_CMD) == 0) {

        // If the first arg is exit, we ignore everything
        // else and exit, just like the UNIX exit.
        return TRUE;
    } else {
        execute_cmd(cmd);
    }
    return FALSE;
}