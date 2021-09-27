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
#include "tok_utils.h"

// Adds tok_utils' token to tokens at token_num.
static bool add_token(int token_num, TokenizeUtils *tok_util, char **tokens);

// Gets the next non-whitespace character in str.
static int get_next_non_whitespace(char *str, int start);

// Gets the end of the token in str.
static int get_end_of_token(char *str, int token_start);

// Checks if curr_char is a redirection symbol.
static bool is_redir(char curr_char);

// Checks if curr_char is whitespace.
static bool is_space(char curr_char);

// Checks if curr_char is a pipe symbol
static bool is_pipe(char curr_char);

// Gets redir in cmd_str and adds it to cmd.
static int get_redir(char *cmd_str, int start, Cmd *cmd);

// Creates new str and copies str from start to end.
static char *str_cpy(char *str, int start, int end);

// Adds tok_utils's token to args at token_num if token has at least one char.
static bool add_token_if_exists(int *token_num, int *token_index, 
        TokenizeUtils *tok_utils, char **args);

// Tokenizes line and stores in cmd.
static int tokenize(Cmd *cmd, TokenizeUtils *tok_utils, int curr_index);

// Deallocates list and tok_utils.
static void dealloc_ll_tok(LList **list, TokenizeUtils **tok_utils);

// Adds cmd from tok_util's line starting at curr_index to cmds_ll.
static int add_cmd(LList *cmds_ll, TokenizeUtils *tok_utils, 
        int curr_index, bool print_on_err);

/*
 * Adds tok_util's token to tokens at token_num.
 * Input: tok_utils whose token which will be added to tokens at token_num.
 *        Will only be added if tok_utils' total_token is less than 
 *        MAX_NUM_TOKENS.
 * Output: TRUE if the add was successful, false otherwise.
 */
static bool add_token(int token_num, TokenizeUtils *tok_util, char **tokens) {
    if (tok_util->total_tokens >= MAX_NUM_TOKENS) {
        print_err();
        return FALSE;
    }
    tokens[token_num] = strdup(tok_util->token);
    tokens[token_num + 1] = NULL;
    if (tokens[token_num] == NULL) {
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
        if (is_space(str[token_start]) || 
            is_redir(str[token_start]) || 
            is_pipe(str[token_start])) {
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

/*
 * Checks if curr_char is a pipe.
 * Input: the current char.
 * Output: True if the current char is a pipe, false otherwise.
 */
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
 * Adds the tok_utils' token to args at token_num if token_index > 0.
 * Input: token_num is where the token will be added at args.
 *        token_index is the index of the end of tok_utils' token.
 * Output: TRUE if successful and FAlSE if any error occurred.
 */
static bool add_token_if_exists(int *token_num, 
            int *token_index, TokenizeUtils *tok_utils, char **args) {
    if (*token_index != 0) {
        tok_utils->token[*token_index] = '\0';
        if (!add_token(*token_num, tok_utils, args)) {
            return FALSE;
        }
        
        // Added token so incr total token and token num. Reset
        // token index.
        (*token_num)++;
        tok_utils->total_tokens += 1;
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
    if (!append_ll(cmd->redirections, redir)) {
        print_err();
        return -1;
    }
    return file_end;
}

/*
 * Splits the line by whitespace, stores tokens, and
 * redirections in a Cmd.
 * Input: A line of tokens seperated by whitespace. curr_index
 *        is where we will start to tokenize the line. tok_utils
 *        contains the current total number of tokens. This is important
 *        since we can only have a fixed maximum number of tokens per line.
 *        tok_utils also contains the token buffer to store tokens that are
 *        being made. Cmd will be populated with args and redirections from
 *        line starting from curr_index.
 * Output: Index of the end of the cmd in line. -1 if error.
 */
static int tokenize(Cmd *cmd, TokenizeUtils *tok_utils, int curr_index) {
    int token_index = 0;
    
    // curr token index in cmd->args.
    int token_num = 0;
    
    while (tok_utils->line[curr_index] != '\0') {
        char curr_char = tok_utils->line[curr_index];
        bool curr_is_pipe = FALSE;
        if (is_space(curr_char) || (curr_is_pipe = is_pipe(curr_char))) {
            // Add the token to the cmds->arg at token_index.
            if (!add_token_if_exists(&token_num, &token_index, tok_utils, cmd->args)) {
                return -1;
            }
            if (curr_is_pipe) {
                return curr_index;
            }
            curr_index++;
        } else {
            // If starts with '<' or '>', create a Redirection instance
            // to hold file name and redir type.
            if (is_redir(curr_char)) {
                if (!add_token_if_exists(&token_num, &token_index, tok_utils, cmd->args) ||
                   (curr_index = get_redir(tok_utils->line, curr_index, cmd)) < 0) {
                    return -1;
                }
                // Add 2 for redir and for file name.
                tok_utils->total_tokens += 2;
            } else {
                // Otherwise, just add regular non whitespace token to args of cmd.
                if (token_index >= MAX_TOKEN_SIZE) {
                    print_err();
                    return -1;
                } 
                tok_utils->token[token_index++] = curr_char;
                curr_index++;
            }
        }
        
    }
    return curr_index;
}

/*
 * Deallocates list and tok_utils.
 * Input: A llist of cmds and tok_utils.
 * Output: NONE.
 */
static void dealloc_ll_tok(LList **list, TokenizeUtils **tok_utils) {
    dealloc_tok_utils(tok_utils);
    dealloc_ll(list, dealloc_cmd);
}

/*
 * Adds a new command to cmds_ll based on the command
 * parsed from tok_utils' line starting at curr_index.
 * If print_on_err, an error message will be printed for 
 * empty commands.
 * Input: A llist of cmds, tok_utils, the curr_index of
 *        the start of the command, and wheter to print
 *        an error message for empty commands.
 * Output: NONE.
 */
static int add_cmd(LList *cmds_ll, TokenizeUtils *tok_utils, int curr_index, bool print_on_err) {
    
    // Create empty command. We can at max have MAX_NUM_TOKENS. If
    // we have already created some commands with tokens, then we
    // can at maximum have a command with MAX_NUM_TOKENS - total number
    // of tokens seen so far.
    Cmd *cmd = create_empty_cmd(MAX_NUM_TOKENS - tok_utils->total_tokens);    
    if (cmd == NULL) {
        print_err();
        return -1;
    }
    
    // Tokenize tok_utils's line from curr_index to the end of the line
    // or until the first pipe seen. Store the results in cmd.
    int last_char = tokenize(cmd, tok_utils, curr_index);
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

    if (!append_ll(cmds_ll, cmd)) {
        print_err();
        dealloc_cmd_specific(cmd);
        return -1;
    }
    return last_char;
}

/*
 * Creates a LList of commands parsed from the line.
 * Input: Line with space seperated tokens. Cmds are 
 *        seperated by pipes.
 * Output: the Cmd or NULL.
 */
LList *get_cmds(char *line) {
    TokenizeUtils *tok_utils = create_tok_utils(line);
    if (tok_utils == NULL) {
        return NULL;
    }
    
    LList *cmds_ll = create_llist();
    if (cmds_ll == NULL) {
        dealloc_tok_utils(&tok_utils);
        return NULL;
    }

    int i = 0;
    while (line[i] != '\0') {
        i = get_next_non_whitespace(line, i);
        if (line[i] == '\0') {
            continue;
        }
        
        // If character is a pipe, create a command that
        // stores all the parsed args/redirections in lines
        // starting at i + 1. Store until the end of line or '|'.
        if (is_pipe(line[i])) {
            tok_utils->total_tokens += 1;
            
            // Prevent '|' at start of line.
            if (is_empty_ll(cmds_ll)) {
                print_err();
                dealloc_ll_tok(&cmds_ll, &tok_utils);
                return NULL;
            }
            
            // We pass TRUE because the cmd should not be empty, 
            // so print err if it is.
            if ((i = add_cmd(cmds_ll, tok_utils, i + 1, TRUE)) < 0) {
                dealloc_ll_tok(&cmds_ll, &tok_utils);
                return NULL;
            }
        
        // The first word in the input should not start with a pipe
        // and the llist should not be allocated yet.
        } else if (!is_empty_ll(cmds_ll)) {
            print_err();
            dealloc_ll_tok(&cmds_ll, &tok_utils);
            return NULL;
        } else {
            // Try to tokenize.
            if ((i = add_cmd(cmds_ll, tok_utils, i, FALSE)) < 0) {
                dealloc_ll_tok(&cmds_ll, &tok_utils);
                return NULL;
            }
        }
    }
    dealloc_tok_utils(&tok_utils);
    return cmds_ll;
}