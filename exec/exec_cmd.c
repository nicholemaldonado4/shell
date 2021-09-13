// Nichole Maldonado
// This file forks() a child
// to execute the provided command.

#include  <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "exec_cmd.h"
#include "shell_consts.h"
#include "str_utils.h"

/*
 * Changes the directory to the dir provided in args
 * Input: args whose first char * is "cd". The second
 *        char * should be the directory.
 * Output: None.
 */
static void change_directory(char **args) {
    if (args[1] == NULL || args[2] != NULL) {
        printf("Shell: Incorrect command to display and prompting for the next command.\n");
        return;
    }
    
    if (chdir(args[1]) < 0) {
        printf("Shell: no such file or directory or directory exists.\n");
        return;
    }
    return;
}

/*
 * Evaluates the exit command stored in args.
 * Input: args which whose first char * points to 
 *        "exit"
 * Output: TRUE if args only contains "exit".
 *         FALSE otherwise.
 */
static bool eval_exit(char **args) {
    if (args[1] != NULL) {
        printf("Shell: Incorrect command to display and prompting for the next command.\n");
        return FALSE;
    }
    return TRUE;
}

/*
 * Gets a potential program path with printMyName 
 * appended to the directory.
 * Input: the prog_name which will be appended to the current
 *        directory.
 * Output: A new string joining the current directory and
 *         prog_name. NULL returned if the result was unsuccessful.
 */
static char *get_cwd_prog(char *prog_name) {
    char cwd[FILENAME_MAX];
    char *possible_prog;
    if (!getcwd(cwd, FILENAME_MAX * sizeof(char)) ||
       (possible_prog = str_copy_and_append(cwd, 0, strlen(cwd), 
                prog_name, strlen(prog_name))) == NULL) {
        return NULL;
    }
    return possible_prog;
}


/*
 * Executes the program.
 * Input: The name of the program/executable and the
 *        corresponding args.
 * Output: NONE.
 */
static void run_executable(char *prog_name, char **args) {
    execvp(prog_name, args);
}

/*
 * Runs the executable and exits() if unsuccessful.
 * Input: the char ** of args for the command.
 * Output: Null.
 */
static void find_run_executable(char **args) {
    run_executable(args[0], args);
    
//    char *possible_prog;
//    if (args[0][0] != '/' && (possible_prog = get_cwd_prog(args[0])) != NULL) {
//        run_executable(possible_prog, args);
//        free(possible_prog);
//    }
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
        find_run_executable(args);
    } else {
        wait(NULL);
    }
    
    return;
}

/*
 * Execute the command in args[0]. Forks
 * a child process to exeucte the command. The
 * parent waits for the child to finish.
 * Input: args which holds the command name
 *        followed by the arguments.
 * Output: Null.
 */
bool execute(char **args) {
    if (strcmp(args[0], CD_CMD) == 0) {
        change_directory(args);
    } else if (strcmp(args[0], EXIT_CMD) == 0) {
        return eval_exit(args);
    } else {
        execute_cmd(args);
    }
    return FALSE;
}