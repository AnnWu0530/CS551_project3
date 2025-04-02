#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "executor.h"
#include "signals.h"
#include "jobs.h"

#define MAX_LINE 1024

int main() {
    char line[MAX_LINE];

    setup_signal_handlers();

    while (1) {
        printf("mysh> ");
        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL) {
            printf("\n");
            break;  // EOF (Ctrl+D)
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0) continue;

        struct command *cmd = parse_line(line);
        if (cmd != NULL) {
            execute_command(cmd);
            free_command(cmd);
        }

        if (strcmp(line, "jobs") == 0) {
            print_jobs();
            continue;
        }

        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
            printf("Goodbye!\n");
            break;
        }
    }

    return 0;
}