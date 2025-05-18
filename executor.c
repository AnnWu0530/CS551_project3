#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include "executor.h"
#include "jobs.h"

pid_t child_pid = -1;

void execute_command(struct command *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Child process

        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);

        execvp(cmd->argv[0], cmd->argv);
        
        // If we get here, execvp failed
        // Use more user-friendly error messages
        if (errno == ENOENT) {
            fprintf(stderr, "%s: command not found\n", cmd->argv[0]);
        } else {
            perror("execvp");
        }
        exit(EXIT_FAILURE);
    } else {
        // Parent
        child_pid = pid;
        if (!cmd->background) {
            int status;
            waitpid(pid, &status, 0);
            child_pid = -1;
        } else {
            add_job(pid, cmd->argv[0]);
            printf("[Background] PID: %d\n", pid);
        }
        child_pid = -1;
    }
}