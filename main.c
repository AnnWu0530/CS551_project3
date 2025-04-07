#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "executor.h"
#include "signals.h"
#include "jobs.h"
#include "history.h" 

#define MAX_LINE 1024

int main() {
    char line[MAX_LINE];

    setup_signal_handlers();
    load_history();

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

        if (strcmp(line, "history") == 0) {
            display_history();
            continue;
        }
        // Handle !n syntax (run the nth command from history)
        if (line[0] == '!' && isdigit(line[1])) {
            int command_number = atoi(&line[1]) - 1;  // Convert to zero-indexed
            run_from_history(command_number);
            continue;
        }
        add_to_history(line);

        struct command *cmd = parse_line(line);
        if (cmd != NULL) {
            execute_command(cmd);
            free_command(cmd);
        }
    }

    save_history();

    return 0;
}