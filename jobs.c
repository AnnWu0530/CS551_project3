#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "jobs.h"
#include <signal.h>

#define MAX_JOBS 128

typedef struct {
    pid_t pid;
    char cmdline[256];
    int stopped;
} job_t;

static job_t jobs[MAX_JOBS];
static int job_count = 0;

int bring_job_to_foreground(int job_id) {
    if (job_id < 1 || job_id > job_count) {
        printf("fg: no such job\n");
        return -1;
    }

    job_t job = jobs[job_id - 1];
    printf("Bringing job [%d] (%d) to foreground: %s\n", job_id, job.pid, job.cmdline);

    if (job.stopped) {
        kill(job.pid, SIGCONT);
        jobs[job_id - 1].stopped = 0;
    }

    int status;
    waitpid(job.pid, &status, 0); 

    remove_job(job.pid);
    return 0;
}

int continue_job_in_background(int job_id) {
    if (job_id < 1 || job_id > job_count) {
        printf("bg: no such job\n");
        return -1;
    }

    job_t *job = &jobs[job_id - 1];
    if (!job->stopped) {
        printf("bg: job already running\n");
        return -1;
    }

    printf("Continuing job [%d] (%d) in background: %s\n", job_id, job->pid, job->cmdline);
    kill(job->pid, SIGCONT);
    job->stopped = 0;

    return 0;
}

void add_job(pid_t pid, const char *cmdline) {
    if (job_count >= MAX_JOBS) return;
    jobs[job_count].pid = pid;
    strncpy(jobs[job_count].cmdline, cmdline, sizeof(jobs[job_count].cmdline)-1);
    job_count++;
}

void remove_job(pid_t pid) {
    for (int i = 0; i < job_count; i++) {
        if (jobs[i].pid == pid) {
            for (int j = i; j < job_count - 1; j++) {
                jobs[j] = jobs[j+1];
            }
            job_count--;
            break;
        }
    }
}

void print_jobs() {
    if (job_count == 0) {
        return;
    }
    for (int i = 0; i < job_count; i++) {
        const char *status = jobs[i].stopped ? "Stopped" : "Running";
        printf("[%d] %d %s [%s]\n", i + 1, jobs[i].pid, jobs[i].cmdline, status);
    }
}

void cleanup_jobs() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            remove_job(pid);
        } else if (WIFSTOPPED(status)) {
            for (int i = 0; i < job_count; i++) {
                if (jobs[i].pid == pid) {
                    jobs[i].stopped = 1;
                    break;
                }
            }
        } else if (WIFCONTINUED(status)) {
            for (int i = 0; i < job_count; i++) {
                if (jobs[i].pid == pid) {
                    jobs[i].stopped = 0;
                    break;
                }
            }
        }
    }
}