#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

void add_job(pid_t pid, const char *cmdline);

void remove_job(pid_t pid);

void print_jobs(void);

void cleanup_jobs(void);

#endif // JOBS_H
