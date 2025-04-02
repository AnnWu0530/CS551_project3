#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "signals.h"

extern pid_t child_pid;

void sig_handler(int signo) {
    if (child_pid > 0) {
        switch (signo) {
            case SIGINT:
                printf("\n[Parent] SIGINT received: interrupting child process...\n");
                kill(child_pid, SIGINT);
                break;
            case SIGTSTP:
                printf("\n[Parent] SIGTSTP received: suspending child process...\n");
                kill(child_pid, SIGTSTP);
                break;
            case SIGQUIT:
                printf("\n[Parent] SIGQUIT received: exiting program...\n");
                if (child_pid > 0) kill(child_pid, SIGKILL);
                exit(0);
        }
    }
}

void setup_signal_handlers() {
    signal(SIGINT, sig_handler);
    signal(SIGTSTP, sig_handler);
    signal(SIGQUIT, sig_handler);
}