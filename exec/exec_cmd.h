// Nichole Maldonado
// This file is a header for exec_cmd which
// forks() a child to execute the provided command.

#ifndef EXEC_CMD_H
#define EXEC_CMD_H

#include "bool.h"
#include "cmd.h"

// Forks() a child to execute the command
// with its args.
bool execute(LList *cmds);

#endif