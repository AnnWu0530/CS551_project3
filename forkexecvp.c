#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t pid;

/* Function to handle the quit signal */
static void sig_usr(int signo) {
    char const *command = "jobs\0";
	switch(signo) {
	   case SIGINT: 
		printf("\nChild process interrupt\n");
		if (pid > 0) kill(pid, SIGSTOP);
		break;
	   case SIGQUIT:
	   	printf("Parent process received quit signal (Control-\\). Exiting...\n");
    	exit(0);
	   case SIGTSTP:
		printf("\nChild process suspended\n");
		if (pid > 0) kill(pid, SIGKILL);
		break;
	   default:
		printf("received signal %d\n", signo);
	}
}

int main(int argc, char **argv) {
    int status;

    if (argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        exit(-1);
    }

    /* Install signal handler for the quit signal */
	if (signal(SIGINT, sig_usr) == SIG_ERR) {
		printf("can't catch SIGINT\n");
		exit(-1);
	}
	if (signal(SIGQUIT, sig_usr) == SIG_ERR) {
		printf("can't catch SIGQUIT\n");
		exit(-1);
	}
	if (signal(SIGTSTP, sig_usr) == SIG_ERR) {
		printf("can't catch SIGTSTP\n");
		exit(-1);
	}

    pid = fork();
    if (pid == 0) { 
        execvp(argv[1], &argv[1]);
        printf("If you see this statement then execvp failed ;-(\n");
    perror("execvp");
    exit(-1);
    } else if (pid > 0) { 
        printf("Wait for the child process to terminate\n");
        while(1) {
            pause();
        }
    } else { 
        perror("fork"); 
        exit(EXIT_FAILURE);
    }

	wait(&status); 
    if (WIFEXITED(status)) { 
        printf("Child process exited with status = %d\n", WEXITSTATUS(status));
    } else { 
        printf("Child process did not terminate normally!\n");
        
    }
    
    printf("[%ld]: Exiting program .....\n", (long)getpid());

    return 0;
}