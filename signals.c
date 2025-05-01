#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "signals.h"
#include "jobs.h"

extern pid_t child_pid;

void sig_handler(int signo) {
    switch (signo) {
        case SIGINT:
            if (child_pid > 0) {
                printf("\n[Shell] SIGINT received: interrupting child process...\n");
                kill(child_pid, SIGINT);
            } else {
                printf("\n[Shell] SIGINT received: no child to interrupt.\n");
            }
            break;

        case SIGTSTP:
            if (child_pid > 0) {
                printf("\n[Shell] SIGTSTP received: suspending child process...\n");
                kill(child_pid, SIGTSTP);
            } else {
                printf("\n[Shell] SIGTSTP received: no child to suspend.\n");
            }
            break;

        case SIGQUIT:
            printf("\n[Shell] SIGQUIT received: exiting shell...\n");
            if (child_pid > 0) kill(child_pid, SIGKILL);
            exit(0);

        case SIGCHLD:
            // Remove zombie processes
            cleanup_jobs();
            break;

        default:
            printf("\n[Shell] Unknown signal received: %d\n", signo);
    }

    // Re-display prompt if needed
    if (signo != SIGQUIT && signo != SIGCHLD) {
        printf("mysh> ");
        fflush(stdout);
    }
}

void setup_signal_handlers() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGCHLD, &sa, NULL);
}