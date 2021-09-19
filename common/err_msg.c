// Nichole Maldonado
// This file prints a standard error message
// for all errors in the minershell.

#include "err_msg.h"

#include <unistd.h>

#define ERR_MSG_LEN 22

/*
 * Prints the default error message to the stderr.
 * Input: None.
 * Output: None.
 */
void print_err() {
    char err_msg[] = "An error has occurred\n";
    write(STDERR_FILENO, err_msg, ERR_MSG_LEN);
}