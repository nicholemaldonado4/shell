// Nichole Maldonado
// This file is a header for exec_cmd which
// fork()s children to execute the provided commands.

#ifndef EXEC_CMD_H
#define EXEC_CMD_H

#include "bool.h"
#include "cmd.h"

// Forks() children to execute the commands.
bool execute(LList *cmds);

#endif