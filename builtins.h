#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h"

// Execute a built-in command if the command matches one
// Returns 1 if a built-in command was executed, 0 otherwise
int execute_builtin(struct command *cmd);

// Built-in command implementations
int builtin_cd(char **args);
int builtin_pwd(char **args);
int builtin_echo(char **args);
int builtin_export(char **args);
int builtin_unset(char **args);
int builtin_exit(char **args);
int builtin_help(char **args);

#endif // BUILTINS_H