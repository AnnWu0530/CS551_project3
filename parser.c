#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

#define MAX_TOKENS 64

struct command *parse_line(const char *line) {
    struct command *cmd = malloc(sizeof(struct command));
    cmd->argv = malloc(sizeof(char*) * MAX_TOKENS);
    cmd->background = 0;

    char *line_copy = strdup(line);
    char *token;
    int i = 0;

    token = strtok(line_copy, " ");
    while (token != NULL && i < MAX_TOKENS - 1) {
        if (strcmp(token, "&") == 0) {
            cmd->background = 1;
        } else {
            cmd->argv[i++] = strdup(token);
        }
        token = strtok(NULL, " ");
    }

    cmd->argv[i] = NULL;
    free(line_copy);

    if (i == 0) {
        free_command(cmd);
        return NULL;
    }

    return cmd;
}

void free_command(struct command *cmd) {
    if (!cmd) return;
    for (int i = 0; cmd->argv[i]; i++) {
        free(cmd->argv[i]);
    }
    free(cmd->argv);
    free(cmd);
}