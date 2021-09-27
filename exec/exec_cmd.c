// Nichole Maldonado
// This file forks() children processes
// to execute the provided commands.

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

// Changes directory to dir.
static bool change_dir(char *dir);

// Starts the cd execution.
static void exec_cd(Cmd *cmd);

// Runs the executable based on args.
static void run_executable(char **args);

// Sets up redirections for the current process.
static bool setup_redirs(LList *redirs, bool set_err_only);

// Sets up child process for execution.
static void setup_child_exec(Node *curr_node, int fd[2], int prev_read_fd);

// Forks children process and connects multiple children with pipes.
static void spawn_execs(LList *cmds);

// Runner code to fork children and wait for forked children.
static void execute_cmds(LList *cmds);

/*
 * Changes the directory to dir.
 * Input: dir that the current directory will be changed to.
 * Output: None.
 */
static bool change_dir(char *dir) {
    // Do not support 'cd' command with no args.
    if (dir == NULL || chdir(dir) < 0) {
        print_err();
        return FALSE;
    }
    return TRUE;
}

/*
 * Changes the directory to the dir provided in args.
 * Input: args whose first char * is "cd". The second
 *        char * should be the directory. If more args
 *        are provided, they are ignored like the unix
 *        "cd" command.
 * Output: None.
 */
static void exec_cd(Cmd *cmd) {
    
    // Assumes STDERR_FILENO exists when we first run the program.
    // Keep copy of old one, setup_redirs may change it. NOTE: In UNIX
    // if we cd with '<' redirections it will just open/close the input file.
    // If the input file does not exist, an error will be printed. For '>'
    // redirection, the outputfile will be open and created if it did not
    // already exist, additionally and standard error will also be written
    // to the file. This part mimcs this.
    int old_err_fd = dup(STDERR_FILENO);
    if (!setup_redirs(cmd->redirections, TRUE)) {
        dup2(old_err_fd, STDERR_FILENO);
        close(old_err_fd);
        return;
    }
    if (!change_dir(cmd->args[1])) {
        dup2(old_err_fd, STDERR_FILENO);
        close(old_err_fd);
        return;
    }
    
    // Restore original standard error.
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
    print_err();
    exit(1);
}

/*
 * Setups all redirs.
 * Input: redirs which is a LList of redirections. If set_error_only
 *        is set to true, then only the stderror will be setup. 
 *        Otherwise, all file descriptors are setup. NOTE:
 *        for the 'cd' command and commands without arguments, UNIX
 *        will still open the files if '<' is included and will create
 *        the files if '>' is included. Thus, we mimic this same behavior if
 *        set_err_only is true.
 * Output: Null.
 */
static bool setup_redirs(LList *redirs, bool set_err_only) {
    Node *redir_node = redirs->head;
    while (redir_node != NULL) {
        Redirection *redir = (Redirection *)redir_node->val;
        
        // For '>' redirections, we open a file of that name,
        // set the STDOUT and STDERR to refer to the file.
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
            // For '<' redirection, we open that file.
            // Set STDIN to refer to it.
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
 * Setups child process for execution.
 *
 * For cmds [0, last_cmd), we create a pipe. For the 0th cmd,
 * we just setup the write end. For the last_cmd, we just setup 
 * the read end. If last_cmd is a single command then it will 
 * not setup read end since no pipe will be created. For all
 * commands between the 0th command and last command, we set its
 * input to come from the previous pipe's read handler (prev_read_fd)
 * and we set the output to the new pipe's write handle.
 *
 * Input: The curr_node that holds the Cmd to execute. fd which
 *        holds the fds for the current pipe. prev_read_fd is the
 *        fd for the previous pipe's read handle.
 * Output: NONE.
 */
static void setup_child_exec(Node *curr_node, int fd[2], int prev_read_fd) {
    // Setup write end for all but the last command since we
    // do not create a pipe for the last end.
    if (curr_node->next != NULL) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        close(fd[0]);
    }
    
    // Setup read end for all but the first command since we do not
    // have a previous pipe for the first command.
    if (prev_read_fd > -1) {
        dup2(prev_read_fd, STDIN_FILENO);
        close(prev_read_fd);
    }
    Cmd *cmd = (Cmd *)curr_node->val;

    // Setup redirections before exec().
    if (!setup_redirs(cmd->redirections, FALSE)) {
        exit(1);
    }
    
    // If the cmd is exit or empty, then ignore it. If
    // cmd is cd still implement it in child process to 
    // follow how UNIX follows cd in pipes. In UNIX the
    // cd command willl still be executed in the child process
    // but we won't see this cd in the parent process. We still
    // use the cd to print an error if the dir does not exist.
    // See test cases for more information.
    if (cmd->args[0] == NULL || 
        strcmp(cmd->args[0], EXIT_CMD) == 0) {
        exit(0);
    } else if (strcmp(cmd->args[0], CD_CMD) == 0) {
        if (!change_dir(cmd->args[1])) {
            exit(1);
        }
        exit(0);
    }
    
    // Otherwise, exec().
    run_executable(cmd->args);
}

/*
 * Execute the commands in cmds. Forks children processes
 * and pipes multiple processes if they exit.
 * Input: cmds which is a LList of Cmds to execute.
 * Output: None.
 */
static void spawn_execs(LList *cmds) {
    int fd[2];
    
    // Will hold the previous read end of a pipe. 
    int prev_read_fd = -1;
    Node *curr_node = cmds->head;
    while (curr_node != NULL) {

        // Create pipes for all but the last command.
        if (curr_node->next != NULL && pipe(fd) < 0) {
            print_err();
            return;
        }
        
        int rc = fork();
        if (rc < 0) {
            print_err();
            return;
        } else if (rc == 0) {
            setup_child_exec(curr_node, fd, prev_read_fd);
        } else {
            // Close all write handlers except for last command since
            // a pipe is not created for the last one.
            if (curr_node->next != NULL) {
                close(fd[1]);
            }
            
            // prev_read_fd will be -1 when we first start off. Only close
            // prev_read_fd when we are closing the previous pipe's read handle.
            if (prev_read_fd >= 0) {
                close(prev_read_fd);
            }
            prev_read_fd = fd[0];
        }
        curr_node = curr_node->next;
    }
}

/*
 * Spawns processes to execute all the cmds.
 * Input: cmds which is a list of Cmds.
 * Output: None.
 */
static void execute_cmds(LList *cmds) {
    spawn_execs(cmds);
    
    // Reap all zombies.
    while (wait(NULL) > 0);
}

/*
 * Executes commands in cmds.
 * Input: cmds which is a list of Cmds.
 * Output: returns TRUE for exit and FALSE otherwise.
 */
bool execute(LList *cmds) {
    if (has_one_node(cmds)) {
        Cmd *cmd = (Cmd *)cmds->head->val;

        // If we only have redirection (no args), then basically open/close 
        // the input/output files. (Treat it as UNIX does, see test cases)
        if (cmd->args[0] == NULL) {

            // Keep track of old STDERR because it can be potentially changed
            // when setting up redirs.
            int old_err_fd = dup(STDERR_FILENO);
            setup_redirs(cmd->redirections, TRUE);

            // Restore old STDERR.
            dup2(old_err_fd, STDERR_FILENO);
            return FALSE;
        } else if (strcmp(cmd->args[0], CD_CMD) == 0) {
            exec_cd(cmd);
            return FALSE;
        } else if (strcmp(cmd->args[0], EXIT_CMD) == 0) {

            // If the first arg is exit, we ignore everything
            // else and exit, just like the UNIX exit.
            return TRUE;
        }
    }
    execute_cmds(cmds);
    return FALSE;
}