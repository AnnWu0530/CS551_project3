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

        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
            printf("Goodbye!\n");
            exit(-1);
        }

        if (strcmp(line, "jobs") == 0) {
            print_jobs();
            continue;
        }

        if (strncmp(line, "fg", 4) == 0) {
            int id = atoi(line + 2);
            bring_job_to_foreground(id);
            continue;
        }
        
        if (strncmp(line, "bg", 4) == 0) {
            int id = atoi(line + 2);
            continue_job_in_background(id);
            continue;
        }

        struct command *cmd = parse_line(line);
        if (cmd != NULL) {
            execute_command(cmd);
            free_command(cmd);
        }
    }

    return 0;
}