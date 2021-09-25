// Nichole Maldonado
// This file takes creates a command
// based on tokens provided.

#include "cmd_parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"
#include "err_msg.h"
#include "shell_consts.h"
#include "str_utils.h"

// Adds token to tokens at token_num.
static bool add_token(int token_num, int total_token, char *token, char **tokens);

// Deallocates cmd and token.
static void dealloc_tok(char **token);

// Initializes token.
static bool init_tok(char **token);

// Gets the next non-whitespace character in str.
static int get_next_non_whitespace(char *str, int start);

// Gets the end of the token in str.
static int get_end_of_token(char *str, int token_start);

// Checks if curr_char is a redirection symbol.
static bool is_redir(char curr_char);

// Checks if curr_char is whitespace.
static bool is_space(char curr_char);

static bool is_pipe(char curr_char);

// Gets redir in cmd_str and adds it to cmd.
static int get_redir(char *cmd_str, int start, Cmd *cmd);

// Creates new str and copies str from start to end.
static char *str_cpy(char *str, int start, int end);

// Adds token to args at token_num if token has at least one char.
static bool add_token_if_exists(int *token_num, int *total_token, 
            int *token_index, char *token, char **args);

// Tokenizes line into a Cmd.
static int tokenize(char *line, Cmd *cmd, char *token, int curr_index);

/*
 * Adds token to tokens at token_num.
 * Input: token which will be added to tokens at token_num.
 *        Will only be added if total_token is less than MAX_NUM_TOKENS.
 * Output: TRUE if the add was successful, false otherwise.
 */
static bool add_token(int token_num, int total_token, char *token, char **tokens) {
    if (total_token >= MAX_NUM_TOKENS) {
        print_err();
        return FALSE;
    }
    tokens[token_num] = strdup(token);
    tokens[token_num + 1] = NULL;
    if (tokens == NULL) {
        print_err();
        return FALSE;
    }
    return TRUE;
}

/*
 * Deallocates cmd and token.
 * Input: a pointer to a cmd and a pointer to a char *.
 * Output: None.
 */
static void dealloc_tok(char **token) {
    free(*token);
    *token = NULL;
}

/*
 * Initializes cmd and token.
 * Input: a pointer to a cmd and a pointer to a char *.
 * Output: true if successful, false if an error occurred while
 *         mallocing.
 */
static bool init_tok(char **token) {
    // Initialize token that will hold the current argument.
    *token = (char *)malloc((MAX_TOKEN_SIZE + 1) * sizeof(char));
    if (*token == NULL) {
        print_err();
        return FALSE;
    }
    return TRUE;
}

/*
 * Gets the index of the next non whitespace char in str.
 * Input: the str and start index to start searching.
 * Output: The index of the next non whitespace char.
 */
static int get_next_non_whitespace(char *str, int start) {
    while (str[start] != '\0' && is_space(str[start])) {
        start++;
    }
    return start;
}

/*
 * Gets the end of a token signified by a whitespace or redirection
 * symbol.
 * Input: the str and the token_start to start looking in the str.
 * Output: the index of the first space or redirection symbol.
 */
static int get_end_of_token(char *str, int token_start) {
    while (str[token_start] != '\0') {
        if (is_space(str[token_start]) || is_redir(str[token_start]) || is_pipe(str[token_start])) {
            return token_start;
        }
        token_start++;
    }
    return token_start;
}

/*
 * Checks if the curr_char is a redirection.
 * Input: the current char.
 * Output: True if the current char is a redirection, false otherwise.
 */
static bool is_redir(char curr_char) {
    return (curr_char == '>' || curr_char == '<');
}


/*
 * Checks if curr_char is a space.
 * Input: the current char.
 * Output: True if the current char is a space, false otherwise.
 */
static bool is_space(char curr_char) {
    return curr_char == ' ' || curr_char == '\n' || curr_char == '\t';
}

static bool is_pipe(char curr_char) {
    return curr_char == '|';
}

/*
 * Mallocs a new str based on str from [start, end).
 * Input: str copied from [start, end).
 * Output: Returns the copied string.
 */
static char *str_cpy(char *str, int start, int end) {
    int str_len = end - start;
    char *new_str = (char *)malloc((str_len + 1) * sizeof(char));
    for (int i = 0; i < str_len; i++) {
        new_str[i] = str[start + i];
    }
    new_str[str_len] = '\0';
    return new_str;
}

/*
 * Adds the token to args at token_num if token_index > 0.
 * Input: token_num is where the token will be added at args.
 *        total_token is the max number of tokens seen. token_index
 *        is the index of the end of token.
 * Output: TRUE if successful and FAlSE if any error occurred.
 */
static bool add_token_if_exists(int *token_num, int *total_token, 
            int *token_index, char *token, char **args) {
    if (*token_index != 0) {
        token[*token_index] = '\0';
        if (!add_token(*token_num, *total_token, token, args)) {
            return FALSE;
        }
        
        // Added token so incr total token and token num. reset
        // token index.
        (*token_num)++;
        (*total_token)++;
        *token_index = 0;
    }
    return TRUE;
}

/*
 * Gets the redirection from cmd_str and adds it to the cmd.
 * Input: The cmd_str where the redirection starts at start.
 *        The redirection will be added to the Cmd.
 * Output: The index of the first character after the redirection
 *         file name. -1 if an error occurred.
 */
static int get_redir(char *cmd_str, int start, Cmd *cmd) {
    // Get the start and and of the file name for the redirection.
    int file_start = get_next_non_whitespace(cmd_str, start + 1);
    if (cmd_str[file_start] == '\0') {
        print_err();
        return -1;
    }
    int file_end = get_end_of_token(cmd_str, file_start + 1);
    char *file_name = str_cpy(cmd_str, file_start, file_end);
    if (file_name == NULL) {
        print_err();
        return -1;
    }

    if (file_end - file_start > MAX_TOKEN_SIZE) {
        print_err();
        return -1;
    }
    
    // Create redirection with the file name and redirection type stored.
    Redirection *redir = create_redirection(file_name, get_redir_type(cmd_str[start]));
    if (redir == NULL) {
        free(file_name);
        return -1;
    }
    
    // Add it the command's llist of redirections. Order of these
    // redirections matter.
    if (append_ll(cmd->redirections, redir) == FALSE) {
        print_err();
        return -1;
    }
    return file_end;
}

/*
 * Splits the line by whitespace, stores tokens, and
 * redirections in a Cmd.
 * Input: A line of tokens seperated by whitespace.
 * Output: A Cmd representation of the line.
 */
static int tokenize(char *line, Cmd *cmd, char *token, int curr_index) {
//    char *token = NULL;
//    if (!init_tok(&token)) {
//        return -1;
//    }
    
//    int i = 0;
    int token_index = 0;
    
    // curr token index in cmd->args.
    int token_num = 0;
    
    // curr num of tokens seen. We cannot have more than MAX_NUM_TOKENS,
    // so this makes sure that it does not exceed the limit. However, 
    // tokens used for redirection are not added to cmd->args, so we 
    // we need a seperate token_num to keep track of the current index
    // that we are adding args to the command.
    int total_token = 0;
    
    while (line[curr_index] != '\0' && !is_pipe(line[curr_index])) {
        char curr_char = line[curr_index];
        if (is_space(curr_char)) {
            // Add the token to the cmds->arg at token_index.
            if (!add_token_if_exists(&token_num, &total_token, &token_index, token, cmd->args)) {
//                dealloc_tok(&token);
                return -1;
            }
            curr_index++;
        } else {
            // If starts with '<' or '>', create a Redirection instance
            // to hold file name and redir type.
            if (is_redir(curr_char)) {
                if (!add_token_if_exists(&token_num, &total_token, &token_index, token, cmd->args) ||
                   (curr_index = get_redir(line, curr_index, cmd)) < 0) {
//                    dealloc_tok(&token);
                    return -1;
                }
                // Add 2 for redir and for file name.
                total_token += 2;
            } else {
                // Otherwise, just add regular non whitespace token to args of cmd.
                if (token_index >= MAX_TOKEN_SIZE) {
                    print_err();
//                    dealloc_tok(&token);
                    return -1;
                } 
                token[token_index++] = curr_char;
                curr_index++;
            }
        }
        
    }
//    free(token);
    return curr_index;
}

static void dealloc_ll_tok(LList **list, char **token) {
    if (*token != NULL) {
        free(*token);
        *token = NULL;
    }
//    if (*cmd != NULL) {
//        dealloc_cmd_specific(*cmd);
//        *cmd = NULL;
//    }
    if (*list != NULL) {
        dealloc_ll(list, dealloc_cmd);
    }
}

static int add_cmd(LList *cmds_ll, char *token, char *line, int curr_index, bool print_on_err) {
    // Create empty command.
    Cmd *cmd = create_empty_cmd();    
    if (cmd == NULL) {
        print_err();
        return -1;
    }

    int last_char = tokenize(line, cmd, token, curr_index);
    if (last_char < 0) {
        dealloc_cmd_specific(cmd);
        return last_char;
    }

    // If cmd does not have args and does not have redirs,
    // deallocate and do nothing.
    if (cmd->args[0] == NULL && is_empty_ll(cmd->redirections)) {
        if (print_on_err) {
            print_err();
        }
        dealloc_cmd_specific(cmd);
        return -1;
    }

    append_ll(cmds_ll, cmd);
    return last_char;
}

/*
 * Creates a Cmd based on the line.
 * Input: Line with space seperated tokens.
 * Output: the Cmd or NULL.
 */
LList *get_cmds(char *line) {
    char *token = NULL;
    if (!init_tok(&token)) {
        return NULL;
    }
    
    LList *cmds_ll = create_llist();
    if (cmds_ll == NULL) {
        free(token);
        return NULL;
    }
    
    int i = 0;
    while (line[i] != '\0') {
        i = get_next_non_whitespace(line, i);
        if (line[i] == '\0') {
            continue;
        }
        
        if (is_pipe(line[i])) {
            if (is_empty_ll(cmds_ll)) {
                print_err();
                dealloc_ll_tok(&cmds_ll, &token);
                return NULL;
            }
            if ((i = add_cmd(cmds_ll, token, line, i + 1, TRUE)) < 0) {
                dealloc_ll_tok(&cmds_ll, &token);
                return NULL;
            }
            
        } else if (!is_empty_ll(cmds_ll)) {
            print_err();
            dealloc_ll_tok(&cmds_ll, &token);
            return NULL;
        } else {
            // Try to tokenize.
            if ((i = add_cmd(cmds_ll, token, line, i, FALSE)) < 0) {
                dealloc_ll_tok(&cmds_ll, &token);
                return NULL;
            }
        }
    }
    free(token);
    return cmds_ll;
}