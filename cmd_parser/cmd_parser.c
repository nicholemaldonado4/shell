#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmd_parser.h"
#include "bool.h"
#include "llist.h"
#include "str_utils.h"
#include "cmd.h"
#include "sub_cmd.h"
#include "shell_consts.h"

static bool create_llist_cmds_args(LList **cmds, LList **args);

static int get_next_non_whitespace(char *str, int start, bool *needs_break);

static bool is_space(char curr_char);

static char *str_cpy(char *str, int start, int end);

static int get_token_add_to_args(char *cmd_str, LList *args, int token_start, bool *needs_break);

static bool is_quoted_char(char *str, int char_index);

static void dealloc_llist_args(LList **cmds, LList **args);

//static bool is_valid_last_sub_cmd(Cmd *cmd);

static bool check_if_unquoted_char(char expected, char *str, int char_index) {
    return str[char_index] == expected && !is_quoted_char(str, char_index);
}

// USED and VERIFIED.
static bool is_redir(char *str, int char_index) {
    return (str[char_index] == '>' || str[char_index] == '<') && !is_quoted_char(str, char_index);
}

static bool is_input_redir(char *str, int char_index) {
    return check_if_unquoted_char('<', str, char_index);
}

static bool is_output_redir(char *str, int char_index) {
    return check_if_unquoted_char('>', str, char_index);
}

static bool is_cmd_ending(char *str, int char_index) {
    return (str[char_index] == ';' || str[char_index] == '&') && !is_quoted_char(str, char_index);
}

static bool is_background_cmd(char *str, int char_index) {
    return check_if_unquoted_char('&', str, char_index);
}

static bool is_pipe(char *str, int char_index) {
    return check_if_unquoted_char('|', str, char_index);
}

// USED and VERIFIEd.
// Rreturns true if the char in str at char_index is preceeded with \.
static bool is_quoted_char(char *str, int char_index) {
    return char_index > 0 && str[char_index - 1] == '\\';
}

// USED and VERIFIED.
static bool is_space(char curr_char) {
    return (curr_char == ' ' || curr_char == '\n' || curr_char == '\t'
        || curr_char == '\v' || curr_char == '\f' || curr_char == '\r');
}

static bool is_quote(char *str, int char_index) {
    return str[char_index] == '"' || check_if_unquoted_char('\'', str, char_index);
}

static char *str_cpy(char *str, int start, int end) {
    int str_len = end - start;
    char *new_str = (char *)malloc((str_len + 1) * sizeof(char));
    for (int i = 0; i < str_len; i++) {
        new_str[i] = str[start + i];
    }
    new_str[str_len] = '\0';
    return new_str;
}

// USED and VERIFIED.
// Returns the index of the end of a token.
static int get_end_of_token(char *str, int token_start) {
    while (str[token_start] != '\0') {
        if (is_space(str[token_start]) || is_output_redir(str, token_start) || 
            is_input_redir(str, token_start) ||
           is_pipe(str, token_start) || is_cmd_ending(str, token_start) || is_background_cmd(str, token_start) || is_quote(str, token_start)) {
            return token_start;
        }
        token_start++;
    }
    return token_start;
}

static int get_token_add_to_args(char *cmd_str, LList *args, int token_start, bool *needs_break) {
    if (*needs_break) {
        printf("Was not expecting '%c' at index %d in line: %s. Please seperate tokens by whitespace or the metacharacters '|', '<', '>', ';', or '&'.", cmd_str[token_start], token_start, cmd_str);
        return -1;
    }
    int token_end = get_end_of_token(cmd_str, token_start + 1);

    char *token = str_cpy(cmd_str, token_start, token_end);

    if (token == NULL) {
        printf("Unable to allocate memory.");
        return -1;
    }
    
    if (!append(args, token)) {
        printf("Unable to allocate memory");
        free(token);
        return -1;
    }
    *needs_break = TRUE;
    return token_end;
}

static int get_next_non_whitespace(char *str, int start, bool *needs_break) {
    while (str[start] != '\0' && is_space(str[start])) {
        *needs_break = FALSE;
        start++;
    }
    return start;
}

// Returns the index of the first instance of quote in str, starting from 
// start.
static int get_end_of_quoted_str(char *str, int start, char quote) {
    while (str[start] != '\0') {
        if (str[start] == quote && !is_quoted_char(str, start)) {
            return start;
        }
        start++;
    }
    return start;
}

static void dealloc_llist_args(LList **cmds, LList **args) {
    dealloc_cmds(cmds);
    dealloc_ll(args, dealloc_str);
}

static void dealloc_nothing() {}

static bool add_args_to_last_sub(Cmd *cmd, LList **args) {
    char **char_list_args = convert_llist_to_char_list(args);
    if (char_list_args == NULL) {
        printf("Unable to allocate memory.");
        return FALSE;
    }
    set_args_for_last_sub(cmd, char_list_args);

    // char_list_args will now refer to the strings in args, so
    // we do not clear the strings, we just dealloc the llist args.
    dealloc_ll(args, dealloc_nothing);
    return TRUE;
}

// Returns the end of a quoted string. token_start is the index of the first quote.
// Returns -1 if error occurred.
static int get_quoted_token_and_add(char *cmd_str, LList *args, int token_start, bool *needs_break) {
    if (*needs_break) {
        printf("Was not expecting '%c' at index %d in line: %s. Please seperate tokens by whitespace or the metacharacters '|', '<', '>', ';', or '&'.", cmd_str[token_start], token_start, cmd_str);
        return -1;
    }
    int token_end = get_end_of_quoted_str(cmd_str, token_start + 1, cmd_str[token_start]);
    if (cmd_str[token_end] == '\0') {
        printf("Was expecting the token to end in '%c' at the end of the line: %s.", cmd_str[token_start], cmd_str);
        return -1;
    }
    
    // Only add non empty arguments to args.
    if (token_start + 1 < token_end) {
        char *new_token = str_cpy(cmd_str, token_start + 1, token_end);
        if (new_token == NULL) {
            printf("Unable to allocate memory\n");
            return -1;
        }
        append(args, new_token);
    }
    *needs_break = TRUE;
    return token_end + 1;
}

static bool has_valid_last_cmd(LList **cmds, LList **curr_args) {
    if (!is_empty(*curr_args)) {
        Cmd *cmd = ((Cmd *)((*cmds)->tail->val));
        if (!add_args_to_last_sub(cmd, curr_args)) {
            dealloc_llist_args(cmds, curr_args);
            return FALSE;
        }
//        if (!is_valid_last_sub_cmd(cmd)) {
//            dealloc_cmds(cmds);
//            return FALSE;
//        }
    } 
    else {
        dealloc_ll(curr_args, dealloc_str);
    }

    if (is_empty(*cmds)) {
        return TRUE;
    }
    
    Cmd *last_cmd = (Cmd *)((*cmds)->tail->val);
    if (last_cmd->sub_cmds->tail == last_cmd->sub_cmds->head && last_cmd->sub_cmds->tail != NULL) {
        SubCmd *last_sub_cmd = ((SubCmd *)(last_cmd->sub_cmds->tail->val));
        if (last_sub_cmd->args == NULL && is_empty(last_sub_cmd->redirections)) {
            delete_last_cmd(cmds);
            return TRUE;
        }
    }
    
    // NOTE: Every cmd will have at least 1 subcmd, so we don't have to check
    // if the last_sub_cmd is NULL.
    SubCmd *last_sub_cmd = ((SubCmd *)(last_cmd->sub_cmds->tail->val));
    if (last_sub_cmd->args == NULL) {
        printf("The command required another command to pipe output to.");
        dealloc_cmds(cmds);
        return FALSE;
    }
    
    
//    Cmd *last_cmd = (Cmd *)((*cmds)->tail->val);
//    SubCmd *last_sub_cmd = ((SubCmd *)(last_cmd->sub_cmds->tail->val));
//    bool removed_last_cmd = FALSE;
//    printf("cmds: \n");
//    print_ll(*cmds, print_cmd);
//
//    // NOTE: Every cmd will have at least 1 subcmd, so we don't have to check
//    // if the last_sub_cmd is NULL.
//    if (last_sub_cmd->args == NULL && is_empty(last_sub_cmd->redirections)) {
//        delete_last_cmd(cmds);
//        removed_last_cmd = TRUE;
//        printf("before\n");
//        print_ll(*cmds, print_cmd);
//        printf("after\n");
//    }
//    if (is_empty(*cmds)) {
//        return TRUE;
//    }
//    last_cmd = (Cmd *)((*cmds)->tail->val);
    
    
//    
//    // Whenever we pipe, we add a new empty subcmd. So if we delete a subcmd and still have
//    // more sub_cmds, then we know that a pipe was issued and never fufilled.
//    if (!removed_last_cmd && last_cmd->sub_cmds->head != NULL) {
//        printf("The command required another command to pipe output to.");
//        dealloc_cmds(cmds);
//        return FALSE;
//    }
    
    return TRUE;
}

static bool add_args_to_last_sub_realloc(Cmd *cmd, LList **args) {
    if (!add_args_to_last_sub(cmd, args)) {
        return FALSE;
    }
    *args = (LList *)malloc(sizeof(LList));
    
    if (*args == NULL) {
        printf("Unable to allocate memory.");
        return FALSE;
    }
    (*args)->head = NULL;
    (*args)->tail = NULL;
    return TRUE;
}

//static bool is_valid_last_sub_cmd(Cmd *cmd) {
//    if (cmd->sub_cmds->tail != NULL && cmd->sub_cmds->tail != cmd->sub_cmds->head) {
//        SubCmd *sub_cmd = (SubCmd *)cmd->sub_cmds->tail->val;   
//        
//        if (sub_cmd->args != NULL && 
//        strcmp(sub_cmd->args[0], EXEC_FROM_FILE) == 0) {
//            printf("Error: The execfromfile command cannot be issued with a pipe.");
//            return FALSE;
//        }
//        
//    }
//    return TRUE;
//}

// USED and VERIFIED.
// Adds args to the last sub cmd in cmds. Then adds a new
// empty Cmd to cmds.
static bool add_cmd(LList *cmds, LList **args, bool *needs_break, bool is_background) {
    *needs_break = FALSE;
    if (is_empty(*args)) {
        printf("Expected a sub command but none were found.");
        return FALSE;
    }
    Cmd *cmd = ((Cmd *)(cmds->tail->val));
    if (!add_args_to_last_sub_realloc(cmd, args)) {
        return FALSE;
    }
    cmd->is_background = is_background;
    
//    if (!is_valid_last_sub_cmd(cmd)) {
//        return FALSE;
//    }

    // Create new Command.
    Cmd *new_cmd = create_empty_cmd();
    if (new_cmd == NULL) {
        printf("Unable to allocate memory");
        return FALSE;
    }
    if (!append(cmds, new_cmd)) {
        printf("Unable to allocate memory");
        dealloc_cmd_specific(new_cmd);
        return FALSE;
    }
    return TRUE;
}


// USED and VERIFIED.
// Allocates memory for a llist of Cmds and adds a blank Cmd to the llist.
// A blank Cmd will have one SubCmd. The SubCmd will have an empty LList of
// redirections. The args for the SubCmd will be NULL. Also creates
// a LList for args.
 static bool create_llist_cmds_args(LList **cmds, LList **args) {
    *cmds = (LList *)malloc(sizeof(LList));
    if (cmds == NULL) {
        printf("Unable to allocate memory.");
        return FALSE;
    }
    (*cmds)->head = NULL;
    (*cmds)->tail = NULL;
    Cmd *cmd = create_empty_cmd();
    if (cmd == NULL) {
        printf("Unable to allocate memory.");
        free(*cmds);
        return FALSE;
    }
   
    if (!append(*cmds, cmd)) {
        printf("Error: Unable to allocate memory.");
        free(*cmds);
        free(cmd);
        cmd = NULL;
        *cmds = NULL;
        return FALSE;
    }
    *args = (LList *)malloc(sizeof(LList));
    if (args == NULL) {
        printf("Error: Unable to allocate memory.");
        free(*cmds);
        cmds = NULL;
    }
    (*args)->head = NULL;
    (*args)->tail = NULL;
    return TRUE;
}

static bool add_pipe(LList *cmds, LList **args, bool *needs_break) {
    *needs_break = FALSE;
    if (is_empty(*args)) {
        printf("Expected a sub command but none was found.");
        return FALSE;
    }
    
//    if (strcmp((char *)((*args)->head->val), EXEC_FROM_FILE) == 0) {
//        printf("Error: The execfromfile cmd cannot be piped.");
//        return FALSE;
//    }

    Cmd *cmd = ((Cmd *)(cmds->tail->val));
    if (!add_args_to_last_sub_realloc(cmd, args)) {
        return FALSE;
    }
    
    // Create a new sub command.
    SubCmd *sub_cmd = create_empty_sub_cmd();
    if (sub_cmd == NULL) {
        printf("Unable to allocate memory.");
        return FALSE;
    }
    if (!append(cmd->sub_cmds, sub_cmd)) {
        printf("Unable to allocate memory");
        dealloc_sub_cmd_specific(sub_cmd);
        return FALSE;
    }
    return TRUE;
}

// Finds and returns the index after the redirection with its file name.
// Saves the redirection to the last cmds, last sub cmd. Returns -1 if 
// error occurred.
static int get_redir_and_add(char *cmd_str, LList *cmds, int token_start, bool *needs_break, RedirType redir_type) {
    int file_start = get_next_non_whitespace(cmd_str, token_start + 1, needs_break);
    int file_end = file_start;
    if (cmd_str[file_start] == '"' || cmd_str[file_start] == '\'') {
        file_end = get_end_of_quoted_str(cmd_str, file_start + 1, cmd_str[file_start]);
        if (cmd_str[file_end] == '\0') {
            printf("Was expecting the token to end in '%c' at the end of the line: %s.", cmd_str[file_start], cmd_str);
            return -1;
        }
        if (file_start + 1 == file_end) {
            printf("File path was not provided for the i/o redirection at index %d.", token_start);
            return -1;
        }
        file_start++;
    } else {
        file_end = get_end_of_token(cmd_str, file_start + 1);
    }
    
    char *file_name = str_cpy(cmd_str, file_start, file_end);
    if (file_name == NULL) {
        printf("Unable to allocate memory.");
        return -1;
    }
    Redirection *redir = create_redirection(file_name, redir_type);
    if (redir == NULL) {
        free(file_name);
        return -1;
    }

    Cmd *cmd = ((Cmd *)(cmds->tail->val));
    if (!add_redir_to_last_sub(cmd, redir)) {
        printf("Unable to allocate memory");
        free(file_name);
        dealloc_redirection_specific(redir);
        return -1;
    }
    *needs_break = TRUE;
    
    return (cmd_str[file_end] == '\'' || cmd_str[file_end] == '"') ? file_end + 1 : file_end;
}

LList *get_list_of_cmds(char *cmd_str) {
    int i = 0;
    bool needs_break = FALSE;
    LList *cmds = NULL;
    LList *curr_args = NULL;
    if (!create_llist_cmds_args(&cmds, &curr_args)) {
        return NULL;
    }
    while (cmd_str[i] != '\0') {
        int token_start = get_next_non_whitespace(cmd_str, i, &needs_break);
        if (cmd_str[token_start] == '\0') {
            i = token_start;
            continue;
        }
        if (cmd_str[token_start] == '"' || cmd_str[token_start] == '\'') {
            i = get_quoted_token_and_add(cmd_str, curr_args, token_start, &needs_break);
            if (i < 0) {
                dealloc_llist_args(&cmds, &curr_args);
                return NULL;
            }
        }
        else if (is_redir(cmd_str, token_start)) {
            i = get_redir_and_add(cmd_str, cmds, token_start, &needs_break, cmd_str[token_start] == '<' ? INPUT : OUTPUT);
            if (i < 0) {
                dealloc_llist_args(&cmds, &curr_args);
                return NULL;
            }
        } else if (is_pipe(cmd_str, token_start)) {
            if (!add_pipe(cmds, &curr_args, &needs_break)) {
                dealloc_llist_args(&cmds, &curr_args);
                return NULL;
            }
            i = token_start + 1;
        } else if (is_cmd_ending(cmd_str, token_start)) {
            if (!add_cmd(cmds, &curr_args, &needs_break, cmd_str[token_start] == '&' ? TRUE : FALSE)) {
                dealloc_llist_args(&cmds, &curr_args);
                return NULL;
            }
            i = token_start + 1;
        } else {
            i = get_token_add_to_args(cmd_str, curr_args, token_start, &needs_break);
            if (i < 0) {
                dealloc_llist_args(&cmds, &curr_args);
                return NULL;
            }
        }
    }
    if (!has_valid_last_cmd(&cmds, &curr_args)) {
        return NULL;
    }
    return cmds;
}