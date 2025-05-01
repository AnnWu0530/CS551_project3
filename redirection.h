#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "parser.h"

// Enhanced command structure that includes redirection and piping
struct command_pipeline {
    int input_fd;           // Input redirection file descriptor
    int output_fd;          // Output redirection file descriptor
    int append;             // Flag for append mode (>>)
    int pipe_count;         // Number of commands in pipeline
    struct command **cmds;  // Array of command structures
};

// Parse a command line with redirection and pipe operators
struct command_pipeline *parse_pipeline(const char *line);

// Free resources used by a command pipeline
void free_pipeline(struct command_pipeline *pipeline);

// Execute a pipeline of commands with redirection
int execute_pipeline(struct command_pipeline *pipeline);

#endif // REDIRECTION_H
