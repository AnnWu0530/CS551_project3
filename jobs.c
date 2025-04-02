#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "jobs.h"

#define MAX_JOBS 128

typedef struct {
    pid_t pid;
    char cmdline[256];
} job_t;

static job_t jobs[MAX_JOBS];
static int job_count = 0;

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
        printf("[%d] %d %s\n", i + 1, jobs[i].pid, jobs[i].cmdline);
    }
}

void cleanup_jobs() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        remove_job(pid);
    }
}