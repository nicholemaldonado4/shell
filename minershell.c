// Nichole Maldonado
// This file runs the main shell, which
// gets commands and executes them.

#include <stdio.h>
#include <stdlib.h>

#include "bool.h"
#include "cmd.h"
#include "cmd_parser.h"
#include "exec_cmd.h"
#include "err_msg.h"
#include "shell_consts.h"
#include "str_utils.h"

// Used to clear last input buffer character.
#define DUMMY_CHAR 'a';

// Parses cmd_str and calls for cmd execution.
static bool parse_and_exec(char *cmd_str);

// Runs the shell.
static void run();

/*
 * Makes the cmd_str tokenized and then calls to
 * execute the commands.
 * Input: cmd_str which is string of tokens.
 *        Tokens are seperated by whitespace.
 * Output: TRUE if it is time to exit the shell
 *         or FALSE otherwise.
 */
static bool parse_and_exec(char *cmd_str) {
    // Tokenize and get cmds.
    LList *cmds = get_cmds(cmd_str);
    if (cmds == NULL || is_empty_ll(cmds)) {
        return FALSE;
    }

    // Run the commands.
    bool end_shell = execute(cmds);
    dealloc_ll(&cmds, dealloc_cmd);
    return end_shell;
}


/*
 * Gets commands from the user until an "exit" command
 * is executed or an EOF is reached. For every line,
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
            print_err();
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
 * Runs main interactive shell that gets user commands,
 * forks children, has the children call the executables, and
 * wait for the children to complete.
 * Input: None.
 * Output: None.
 */
int main() {
    run();
    return 0;
}