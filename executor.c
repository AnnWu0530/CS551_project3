#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "executor.h"

pid_t child_pid = -1;

void execute_command(struct command *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Child process
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Parent
        child_pid = pid;
        if (!cmd->background) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            printf("[Background] PID: %d\n", pid);
        }
        child_pid = -1;
    }
}
