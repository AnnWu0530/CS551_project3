#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t pid;

volatile sig_atomic_t child_pid = -1;

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

void run_command(char *cmd) {
    child_pid = fork();

    if (child_pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // child
        execlp(cmd, cmd, (char *)NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else {
        // parent
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("[Parent] Child '%s' exited with status %d\n", cmd, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[Parent] Child '%s' terminated by signal %d\n", cmd, WTERMSIG(status));
        }
        child_pid = -1;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <command1> <command2>\n", argv[0]);
        return EXIT_FAILURE;
    }

    setup_signal_handlers();

    printf("[Parent] Starting execution of '%s'\n", argv[1]);
    run_command(argv[1]);

    printf("[Parent] Starting execution of '%s'\n", argv[2]);
    run_command(argv[2]);

    printf("[Parent] All commands processed. Waiting for quit signal (Ctrl-\\)...\n");

    // Just wait forever until user sends SIGQUIT
    while (1) {
        pause();
    }

    return 0;
}