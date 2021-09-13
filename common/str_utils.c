// Nichole Maldonado
// This file contains various string utils.

#include <stdio.h>
#include <stdlib.h>
#include "str_utils.h"

/* 
 * Creates a new str that includes all the characters of str
 * from [start, end) and all of append_str. Joins the two strings
 * by the character at the start.
 * Input: str which is the start of the string and append_str
 *        which is added to the end of str. We append append_str
 *        to str from start to end. The length of append_str
 *        is also provided.
 * Output: A newly allocated string of str joined to append_str.
 */
char *str_copy_and_append(char *str, int start, int end, 
        const char *append_str, int append_str_len) {    
    if (end <= start) {
        return NULL;
    }
    char joiner = str[start];
    if (str[end - 1] == joiner) {
        end--;
    }
    
    char *new_str = (char *)malloc((end - start + append_str_len + 2) * sizeof(char));
    if (new_str == NULL) {
        return NULL;
    }
    int sub_str_len = end - start;
    for (int i = 0; i < sub_str_len; i++) {
        new_str[i] = str[i + start];
    }
    new_str[sub_str_len] = joiner;
    for (int i = 0; i < append_str_len; i++) {
        new_str[i + sub_str_len + 1] = append_str[i];
    }
    new_str[sub_str_len + append_str_len + 1] = '\0';
    return new_str;
}

/* 
 * Deallocates a char **strs.
 * Input: A pointer to an array of char pointers.
 *        Each char * is pointing a malloced string.
 *        strs in NULL terminated.
 * Output: None.
 */
void dealloc_str_arr(char ***strs) {
    char **curr_str = *strs;
    int i = 0;
    while (curr_str[i] != NULL) {
        free(curr_str[i]);
        i++;
    }
    free(*strs);
}