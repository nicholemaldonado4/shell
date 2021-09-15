// Nichole Maldonado
// This file forks() a child
// to execute the provided command.

#include  <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "exec_cmd.h"
#include "shell_consts.h"
#include "str_utils.h"

// Changes directory.
static void change_directory(char **args);

// Runs the executable based on args.
static void run_executable(char **args);

// Forks child to run executable.
static void execute_cmd(char **args);


/*
 * Changes the directory to the dir provided in args.
 * Input: args whose first char * is "cd". The second
 *        char * should be the directory. If more args
 *        are provided, they are ignored like the unix
 *        "cd" command.
 * Output: None.
 */
static void change_directory(char **args) {
    if (args[1] == NULL) {
        printf("Shell: Incorrect command to display and prompting for the next command.\n");
        return;
    }
    
    if (chdir(args[1]) < 0) {
        printf("Shell: cd: %s: No such file or directory\n", args[1]);
        return;
    }
    return;
}

/*
 * Runs the executable and exits() if unsuccessful.
 * Input: the char ** of args for the command.
 * Output: Null.
 */
static void run_executable(char **args) {
    execvp(args[0], args);
    perror("Shell: %s");
    printf("Shell: %s: command not found\n", args[0]);
    exit(1);
}

/*
 * Execute the command in args[0]. Forks
 * a child process to exeucte the command. The
 * parent waits for the child to finish.
 * Input: args which holds the command name
 *        followed by the arguments.
 * Output: Null.
 */
static void execute_cmd(char **args) {
    int rc = fork();
    if (rc < 0) {
        printf("Shell: Unable to fork.\n");
        return;
    } else if (rc == 0) {
        run_executable(args);
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
bool execute(char **args) {
    if (strcmp(args[0], CD_CMD) == 0) {
        change_directory(args);
    } else if (strcmp(args[0], EXIT_CMD) == 0) {
        // If the first arg is exit, we ignore everything
        // else and exit, just like the UNIX exit.
        return TRUE;
    } else {
        execute_cmd(args);
    }
    return FALSE;
}