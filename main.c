#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "signals.h"
#include "parser.h"

#define MAX_LINE 1024

int main() {
    char line[MAX_LINE];
    char *argv[64];

    setup_signal_handlers();

    while (1) {
        printf("mysh> ");
        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0) continue;

        // Special command to quit
        if (strcmp(line, "exit") == 0) break;

        // parse into argv
        parse_command(line, argv);

        pid_t pid = fork();
        if (pid == 0) {
            execvp(argv[0], argv);
            perror("execvp failed");
            exit(1);
        } else {
            waitpid(pid, NULL, 0);
        }
    }

    printf("Bye!\n");
    return 0;
}