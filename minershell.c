// Nichole Maldonado
// This file runs the main shell, which
// gets a command and executes it.

#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "cmd.h"
#include "exec_cmd.h"
#include "shell_consts.h"
#include "str_utils.h"
#include "cmd_parser.h"

// Used to clear last input buffer character.
#define DUMMY_CHAR 'a';

// Parses cmd_str and calls for cmd execution.
static bool parse_and_exec(char *cmd_str);

// Runs the shell.
static void run();

/*
 * Makes the cmd_str tokenized and then calls to
 * execute the command.
 * Input: cmd_str which is string of tokens.
 *        Tokens are seperated by whitespace.
 * Output: TRUE if it is time to exit the shell
 *         or FALSE otherwise.
 */
static bool parse_and_exec(char *cmd_str) {
    Cmd *cmd = get_cmd(cmd_str);
    if (cmd == NULL) {
        return FALSE;
    }
    bool end_shell = execute(cmd);
    dealloc_cmd_specific(cmd);
    return end_shell;
}


/*
 * Gets a command from the user until an "exit" command
 * is executed or an EOF is reached. For every command,
 * it sent to parsed and executed.
 * Input: None.
 * Output: None.
 */
static void run() {
    bool exit_shell = FALSE;
    
    // MAX_INPUT + 1 because fgets() will always add '/0' to the last char.
    // So at max we allow MAX_INPUT_SIZE chars and we leave extra space
    // to end the input.
    char user_input[MAX_INPUT_SIZE + 1];
    
    // We assign the last character to a dummy char after each fgets
    // because we always check to see if the last char is '\0' to
    // see if an overflow occurred.
    user_input[MAX_INPUT_SIZE] = DUMMY_CHAR;
    while (!exit_shell) {

        printf("%s$ ", SHELL_PROMPT);
        if (fgets(user_input, MAX_INPUT_SIZE + 1, stdin) == NULL) {
            printf("\n");
            return;
        }
        
        if (user_input[MAX_INPUT_SIZE] == '\0' &&
           user_input[MAX_INPUT_SIZE - 1] != '\n') {
            printf("Shell: Input exceeded max limit.\n");
            scanf("%*[^\n]");
            scanf("%*c");
        } else {
            exit_shell = parse_and_exec(user_input);
        }
        user_input[0] = '\0';
        user_input[MAX_INPUT_SIZE] = DUMMY_CHAR;
    }
}

/*
 * Runs main interactive shell that gets a user command,
 * forks a child, has the child call the executable, and
 * wait for the child to complete.
 * Input: None.
 * Output: None.
 */
int main() {
    run();
    return 0;
}