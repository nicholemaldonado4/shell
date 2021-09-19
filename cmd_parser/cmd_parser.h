// Nichole Maldonado
// This file is the header for a tokenizer
// which takes a string, splits it by 
// whitespace, and creates a command.

#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include "cmd.h"

// Splits line by whitespace and returns its 
// Cmd representation.
Cmd *get_cmd(char *line);

#endif