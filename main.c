#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "executor.h"
#include "signals.h"
#include "jobs.h"
#include "history.h"
#include "env.h"
#include "builtins.h"
#include "redirection.h"
#include "completion.h"
#include "config.h"
#include "glob.h"
#include "alias.h"
#include "error.h"

#define MAX_LINE 1024

int main() {
    char line[MAX_LINE];
    char *prompt = "mysh> ";

    // Initialize all modules
    error_set_log_level(LOG_INFO);
    setup_signal_handlers();
    env_init();
    config_init();
    alias_init();
    completion_init();
    load_history();
    
    // Check if custom prompt is set in config
    char *custom_prompt = config_get("prompt");
    if (custom_prompt) {
        prompt = custom_prompt;
    }

    while (1) {
        printf("%s", prompt);
        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL) {
            printf("\n");
            break;  // EOF (Ctrl+D)
        }

        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0) continue;

        // Add command to history
        add_to_history(line);

        // Handle built-in exit/quit command directly for simplicity
        if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
            printf("Goodbye!\n");
            break;
        }

        // Handle jobs command directly for simplicity
        if (strcmp(line, "jobs") == 0) {
            print_jobs();
            continue;
        }

        // Handle fg command directly for simplicity
        if (strncmp(line, "fg", 2) == 0 && (line[2] == ' ' || line[2] == '\0')) {
            int id = 0;
            if (line[2] != '\0') {
                id = atoi(line + 3);
            }
            bring_job_to_foreground(id > 0 ? id : 1);
            continue;
        }
        
        // Handle bg command directly for simplicity
        if (strncmp(line, "bg", 2) == 0 && (line[2] == ' ' || line[2] == '\0')) {
            int id = 0;
            if (line[2] != '\0') {
                id = atoi(line + 3);
            }
            continue_job_in_background(id > 0 ? id : 1);
            continue;
        }

        // Handle history command directly for simplicity
        if (strcmp(line, "history") == 0) {
            display_history();
            continue;
        }

        // Handle !n syntax (run the nth command from history)
        if (line[0] == '!' && isdigit(line[1])) {
            int command_number = atoi(&line[1]);
            run_from_history(command_number - 1);  // Convert to zero-indexed
            continue;
        }

        // Handle alias command
        if (strncmp(line, "alias", 5) == 0) {
            if (line[5] == '\0') {
                // List all aliases
                alias_list();
            } else {
                // Define a new alias
                char *name = strtok(line + 6, "=");
                char *value = strtok(NULL, "");
                
                if (name && value) {
                    // Remove leading/trailing whitespace and quotes
                    while (*name && isspace(*name)) name++;
                    char *end = name + strlen(name) - 1;
                    while (end > name && isspace(*end)) *end-- = '\0';
                    
                    // Remove surrounding quotes from value if present
                    if (value[0] == '\'' || value[0] == '"') {
                        value++;
                        end = value + strlen(value) - 1;
                        if (*end == '\'' || *end == '"') *end = '\0';
                    }
                    
                    alias_define(name, value);
                } else {
                    error_log(LOG_ERROR, "Usage: alias name='value'");
                }
            }
            continue;
        }

        // Handle unalias command
        if (strncmp(line, "unalias", 7) == 0 && line[7] != '\0') {
            char *name = line + 8;
            while (*name && isspace(*name)) name++;  // Skip whitespace
            
            if (alias_remove(name) != 0) {
                error_log(LOG_ERROR, "unalias: %s not found", name);
            }
            continue;
        }

        // Expand aliases
        char *expanded_alias = alias_expand(line);
        if (!expanded_alias) {
            error_log(LOG_ERROR, "Failed to expand aliases");
            continue;
        }

        // Expand wildcards
        char *expanded_wildcards = expand_wildcards(expanded_alias);
        if (!expanded_wildcards) {
            error_log(LOG_ERROR, "Failed to expand wildcards");
            free(expanded_alias);
            continue;
        }

        // Expand environment variables
        char *expanded_env = env_expand(expanded_wildcards);
        if (!expanded_env) {
            error_log(LOG_ERROR, "Failed to expand environment variables");
            free(expanded_wildcards);
            free(expanded_alias);
            continue;
        }

        // Try parsing as simple command first
        struct command *cmd = parse_line(expanded_env);
        if (cmd) {
            // Try to execute as a built-in command first
            if (execute_builtin(cmd)) {
                // Built-in command was executed, clean up and continue
                free_command(cmd);
                free(expanded_env);
                free(expanded_wildcards);
                free(expanded_alias);
                continue;
            }

            // Not a built-in, execute as external command
            execute_command(cmd);
            free_command(cmd);
        } else {
            // Parse as pipeline if simple command parsing failed
            struct command_pipeline *pipeline = parse_pipeline(expanded_env);
            if (pipeline) {
                // Try to execute first command as built-in if it's not a pipeline
                if (pipeline->pipe_count == 1 && execute_builtin(pipeline->cmds[0])) {
                    // Built-in command was executed
                    free_pipeline(pipeline);
                } else {
                    // Execute as pipeline
                    execute_pipeline(pipeline);
                    free_pipeline(pipeline);
                }
            } else {
                error_log(LOG_ERROR, "Failed to parse command");
            }
        }

        // Clean up
        free(expanded_env);
        free(expanded_wildcards);
        free(expanded_alias);
    }

    // Clean up and save state
    save_history();
    alias_cleanup();
    config_cleanup();
    env_cleanup();
    completion_cleanup();

    return 0;
}