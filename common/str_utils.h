// Nichole Maldonado
// This file is a header for various string
// utils.

#ifndef STR_UTILS_H
#define STR_UTILS_H

// Creates a new string that appends append_str to
// the end of str. The strings are joined by the first
// character in str.
char *str_copy_and_append(char *str, int start, int end, 
        const char *append_str, int append_str_len);

// Deallocates an array of char *s.
void dealloc_str_arr(char ***strs);

#endif