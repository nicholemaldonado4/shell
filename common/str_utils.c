// Nichole Maldonado
// This file contains various string utils.

#include <stdio.h>
#include <stdlib.h>
#include "str_utils.h"

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