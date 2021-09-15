#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include "llist.h"
#include "bool.h"
#include "str_utils.h"
#include "exec_from_file.h"
#include "cmd.h"
#include "cmd_parser.h"

static const int MAX_BUFFER_SIZE = 200;

static char *join(char *str1, char *str2, char join_char) {
    int str1_len = strlen(str1);
    int str2_len = strlen(str2);
    char *new_str;
    bool had_join = FALSE;
    if (str1[str1_len - 1] == join_char) {
        had_join = TRUE;
        new_str = (char *)malloc(str1_len + str2_len + 1);
    } else {
        new_str = (char *)malloc(str1_len + str2_len + 2);
    }
    
    if (new_str == NULL) {
        printf("Error: Unable to allocate memory.\n");
        return NULL;
    }
    
    for (int i = 0; i < str1_len; i++) {
        new_str[i] = str1[i];
    }
    int base_start = str1_len;
    if (!had_join) {
        new_str[base_start] = join_char;
        base_start++;
    }
    for (int i = 0; i < str2_len; i++) {
        new_str[base_start + i] = str2[i];
    }
    new_str[base_start + str2_len] = '\0';
    return new_str;
}

static LList *read_from_file(char *file_path) {
    int fd;
    if ((fd = open(file_path, O_RDONLY)) < 0) {
        printf("Error: Unable to open file %s", file_path);
        return NULL;
    }
    LList *lines = (LList *)malloc(sizeof(LList));
    lines->head = NULL;
    lines->tail = NULL;
    char buffer[MAX_BUFFER_SIZE];
    int i = 0;
    int num_bytes = 0;
    while (i < (MAX_BUFFER_SIZE - 1) && (num_bytes = read(fd, (buffer + i), 1)) > 0) {
        if (buffer[i] == '\n') {
            buffer[i] = '\0';
            char *file_lines = strdup(buffer);
            if (file_lines == NULL) {
                printf("Error: Unable to allocate memory.\n");
                dealloc_ll(&lines, dealloc_str);
                return NULL;
            }
            append(lines, file_lines);
            buffer[0] = '\0';
            i = 0;
        } else {
            i++;
        }        
    }
    if (i >= MAX_BUFFER_SIZE - 1) {
        printf("Error: the file had too long file lines. Reshorten the lines and try again.\n");
        dealloc_ll(&lines, dealloc_str);
        return NULL;
    }
    
    if (num_bytes != 0) {
        printf("Error: A problem occurred while reading the file\n");
        dealloc_ll(&lines, dealloc_str);
        return NULL;
    }
    return lines;
}

static int get_next_non_whitespace(char *str, int start) {
    while (str[start] != '\0' && isspace(str[start])) {
        start++;
    }
    return start;
}

static LList *get_file_cmds(char *file_path) {
    char *abs_file_path = file_path;
    bool create_abs_file = FALSE;
    if (file_path[0] != '/') {
        create_abs_file = TRUE;
        char cwd[FILENAME_MAX];
        char *cwd_str = getcwd(cwd, FILENAME_MAX * sizeof(char));
        if (cwd_str == NULL) {
            cwd_str = "";
        }
        
        abs_file_path = join(cwd_str, file_path, '/');
        if (abs_file_path == NULL) {
            return NULL;
        }
    }
    LList *cmds = (LList *)malloc(sizeof(LList));
    if (cmds == NULL) {
        if (create_abs_file) {
            free(abs_file_path);
        }
        return NULL;
    }
    
    cmds->head = NULL;
    cmds->tail = NULL;
    
    LList *file_lines = read_from_file(abs_file_path);
    free(abs_file_path);
    abs_file_path = NULL;

    if (file_lines == NULL) {
        return NULL;
    }
    Node *file_node = file_lines->head;
    while (file_node != NULL) {
        char *line = (char *)file_node->val;
        int start_token = get_next_non_whitespace(line, 0);
        if (line[start_token] != '#' && line[start_token] != '\0') {
            LList *parsed_cmds = get_list_of_cmds(line);
            if (parsed_cmds == NULL) {
                dealloc_ll(&file_lines, dealloc_str);
                dealloc_cmds(&cmds);
                return NULL;
            }
            append_ll(cmds, &parsed_cmds);
        }
        file_node = file_node->next;
    }
    dealloc_ll(&file_lines, dealloc_str);
    return cmds;
}

// Assumes sub_cmd[0] == "execfromfile"
LList *exec_from_file(SubCmd *sub_cmd) {
    if ((sub_cmd->args)[1] == NULL || (sub_cmd->args)[2] != NULL) {
        printf("Error: command execfromfile can only take a file path as an argument.\n");
        return NULL;
    }
    return get_file_cmds(sub_cmd->args[1]);
}
