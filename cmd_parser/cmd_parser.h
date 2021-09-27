// Nichole Maldonado
// This file is the header for a tokenizer
// which takes a string, splits it by 
// whitespace, and creates a list of commands.

#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include "cmd.h"

// Parses cmds from line and stores in a LList.
LList *get_cmds(char *line);

#endif