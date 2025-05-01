#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include "redirection.h"
#include "jobs.h"
#include "env.h"
#include "builtins.h"

extern pid_t child_pid;

// Tokenize a string by delimiters
static char **tokenize(const char *str, const char *delim, int *count) {
    char *str_copy = strdup(str);
    if (!str_copy) return NULL;
    
    // Count tokens
    int num_tokens = 0;
    char *p = str_copy;
    char *token;
    
    while ((token = strsep(&p, delim)) != NULL) {
        if (*token != '\0') {  // Skip empty tokens
            num_tokens++;
        }
    }
    
    // Allocate array for tokens
    char **tokens = malloc((num_tokens + 1) * sizeof(char*));
    if (!tokens) {
        free(str_copy);
        return NULL;
    }
    
    // Reset and tokenize again
    free(str_copy);
    str_copy = strdup(str);
    p = str_copy;
    int i = 0;
    
    while ((token = strsep(&p, delim)) != NULL) {
        if (*token != '\0') {  // Skip empty tokens
            tokens[i++] = strdup(token);
        }
    }
    tokens[i] = NULL;
    
    *count = num_tokens;
    free(str_copy);
    return tokens;
}

// Parse a command segment (between pipes)
static struct command *parse_command_segment(const char *segment) {
    struct command *cmd = malloc(sizeof(struct command));
    if (!cmd) return NULL;
    
    cmd->argv = NULL;
    cmd->background = 0;
    
    char *segment_copy = strdup(segment);
    char *input_file = NULL;
    char *output_file = NULL;
    // Removed unused variable: int append = 0;
    
    // Tokenize by spaces
    int token_count;
    char **tokens = tokenize(segment_copy, " \t", &token_count);
    if (!tokens) {
        free(segment_copy);
        free(cmd);
        return NULL;
    }
    
    // Parse tokens and build command
    cmd->argv = malloc((token_count + 1) * sizeof(char*));
    if (!cmd->argv) {
        for (int i = 0; tokens[i]; i++) free(tokens[i]);
        free(tokens);
        free(segment_copy);
        free(cmd);
        return NULL;
    }
    
    int arg_index = 0;
    for (int i = 0; i < token_count; i++) {
        if (strcmp(tokens[i], "<") == 0 && i + 1 < token_count) {
            // Input redirection
            input_file = strdup(tokens[++i]);
        } else if (strcmp(tokens[i], ">") == 0 && i + 1 < token_count) {
            // Output redirection
            output_file = strdup(tokens[++i]);
            // Removed: append = 0;
        } else if (strcmp(tokens[i], ">>") == 0 && i + 1 < token_count) {
            // Output redirection with append
            output_file = strdup(tokens[++i]);
            // Removed: append = 1;
        } else if (strcmp(tokens[i], "&") == 0) {
            // Background execution
            cmd->background = 1;
        } else {
            // Regular argument
            cmd->argv[arg_index++] = strdup(tokens[i]);
        }
    }
    cmd->argv[arg_index] = NULL;
    
    // Clean up
    for (int i = 0; tokens[i]; i++) free(tokens[i]);
    free(tokens);
    free(segment_copy);
    
    // TODO: Store input/output redirections in command structure
    // For now, just free them
    if (input_file) free(input_file);
    if (output_file) free(output_file);
    
    return cmd;
}

// Parse a command line with redirection and pipe operators
struct command_pipeline *parse_pipeline(const char *line) {
    if (!line) return NULL;
    
    struct command_pipeline *pipeline = malloc(sizeof(struct command_pipeline));
    if (!pipeline) return NULL;
    
    pipeline->input_fd = STDIN_FILENO;
    pipeline->output_fd = STDOUT_FILENO;
    pipeline->append = 0;
    pipeline->pipe_count = 0;
    pipeline->cmds = NULL;
    
    // Split by pipe character
    int segment_count;
    char **segments = tokenize(line, "|", &segment_count);
    if (!segments) {
        free(pipeline);
        return NULL;
    }
    
    pipeline->pipe_count = segment_count;
    pipeline->cmds = malloc(segment_count * sizeof(struct command*));
    if (!pipeline->cmds) {
        for (int i = 0; segments[i]; i++) free(segments[i]);
        free(segments);
        free(pipeline);
        return NULL;
    }
    
    // Parse each command segment
    for (int i = 0; i < segment_count; i++) {
        pipeline->cmds[i] = parse_command_segment(segments[i]);
        if (!pipeline->cmds[i]) {
            // Clean up on error
            for (int j = 0; j < i; j++) {
                free_command(pipeline->cmds[j]);
            }
            for (int j = 0; segments[j]; j++) free(segments[j]);
            free(segments);
            free(pipeline->cmds);
            free(pipeline);
            return NULL;
        }
    }
    
    // Clean up
    for (int i = 0; segments[i]; i++) free(segments[i]);
    free(segments);
    
    return pipeline;
}

// Execute a pipeline of commands with redirection
int execute_pipeline(struct command_pipeline *pipeline) {
    if (!pipeline || pipeline->pipe_count == 0) return -1;
    
    // Simple case: just one command
    if (pipeline->pipe_count == 1) {
        struct command *cmd = pipeline->cmds[0];
        
        // Try to execute as builtin first
        if (execute_builtin(cmd)) {
            return 0;
        }
        
        // Set up file redirections
        int saved_stdin = -1, saved_stdout = -1;
        
        // Handle input redirection
        if (pipeline->input_fd != STDIN_FILENO) {
            saved_stdin = dup(STDIN_FILENO);
            dup2(pipeline->input_fd, STDIN_FILENO);
            close(pipeline->input_fd);
        }
        
        // Handle output redirection
        if (pipeline->output_fd != STDOUT_FILENO) {
            saved_stdout = dup(STDOUT_FILENO);
            dup2(pipeline->output_fd, STDOUT_FILENO);
            close(pipeline->output_fd);
        }
        
        // Execute the command
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return -1;
        }
        
        if (pid == 0) {
            // Child process
            execvp(cmd->argv[0], cmd->argv);
            
            // If we get here, execvp failed
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
        
        // Restore redirections
        if (saved_stdin != -1) {
            dup2(saved_stdin, STDIN_FILENO);
            close(saved_stdin);
        }
        if (saved_stdout != -1) {
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }
        
        return 0;
    }
    
    // Multiple commands: set up pipes
    int pipes[pipeline->pipe_count - 1][2];
    for (int i = 0; i < pipeline->pipe_count - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            return -1;
        }
    }
    
    // Launch all processes
    pid_t pids[pipeline->pipe_count];
    for (int i = 0; i < pipeline->pipe_count; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            return -1;
        }
        
        if (pids[i] == 0) {
            // Child process
            
            // Set up input from previous pipe (if not first command)
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            } else if (pipeline->input_fd != STDIN_FILENO) {
                dup2(pipeline->input_fd, STDIN_FILENO);
                close(pipeline->input_fd);
            }
            
            // Set up output to next pipe (if not last command)
            if (i < pipeline->pipe_count - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            } else if (pipeline->output_fd != STDOUT_FILENO) {
                dup2(pipeline->output_fd, STDOUT_FILENO);
                close(pipeline->output_fd);
            }
            
            // Close all pipe file descriptors
            for (int j = 0; j < pipeline->pipe_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            // Execute the command
            execvp(pipeline->cmds[i]->argv[0], pipeline->cmds[i]->argv);
            
            // If we get here, execvp failed
            if (errno == ENOENT) {
                fprintf(stderr, "%s: command not found\n", pipeline->cmds[i]->argv[0]);
            } else {
                perror("execvp");
            }
            exit(EXIT_FAILURE);
        }
    }
    
    // Parent: close all pipe file descriptors
    for (int i = 0; i < pipeline->pipe_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    // Wait for all processes (if not background)
    if (!pipeline->cmds[pipeline->pipe_count - 1]->background) {
        for (int i = 0; i < pipeline->pipe_count; i++) {
            if (i == pipeline->pipe_count - 1) {
                child_pid = pids[i];
            }
            
            int status;
            waitpid(pids[i], &status, 0);
            
            if (i == pipeline->pipe_count - 1) {
                child_pid = -1;
            }
        }
    } else {
        // Add last process to job list if background
        add_job(pids[pipeline->pipe_count - 1], pipeline->cmds[pipeline->pipe_count - 1]->argv[0]);
        printf("[Background] PID: %d\n", pids[pipeline->pipe_count - 1]);
    }
    
    return 0;
}

// Free resources used by a command pipeline
void free_pipeline(struct command_pipeline *pipeline) {
    if (!pipeline) return;
    
    if (pipeline->cmds) {
        for (int i = 0; i < pipeline->pipe_count; i++) {
            free_command(pipeline->cmds[i]);
        }
        free(pipeline->cmds);
    }
    
    // Close any open file descriptors
    if (pipeline->input_fd != STDIN_FILENO) {
        close(pipeline->input_fd);
    }
    if (pipeline->output_fd != STDOUT_FILENO) {
        close(pipeline->output_fd);
    }
    
    free(pipeline);
}