#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include "completion.h"
#include "env.h"

#define MAX_MATCHES 100

// Initialize tab completion
void completion_init(void) {
    // Nothing to initialize for basic completion
}

// Check if a given path is a directory
static int is_directory(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return 0;
}

// Find the start of the current word to complete
static char *find_current_word(const char *line, int cursor_pos) {
    // Create a copy of the line up to the cursor position
    char *partial_line = malloc(cursor_pos + 1);
    if (!partial_line) return NULL;
    
    strncpy(partial_line, line, cursor_pos);
    partial_line[cursor_pos] = '\0';
    
    // Find the last space or the beginning of the line
    char *last_space = strrchr(partial_line, ' ');
    if (last_space) {
        return strdup(last_space + 1);
    } else {
        return strdup(partial_line);
    }
}

// Get completions for a command (search in PATH)
static char **get_command_completions(const char *partial, int *count) {
    char **matches = malloc(MAX_MATCHES * sizeof(char *));
    if (!matches) return NULL;
    
    *count = 0;
    
    // Get the PATH environment variable
    char *path_env = env_get("PATH");
    if (!path_env) return matches;
    
    char *path_copy = strdup(path_env);
    char *path_token = strtok(path_copy, ":");
    
    while (path_token && *count < MAX_MATCHES) {
        DIR *dir = opendir(path_token);
        if (dir) {
            struct dirent *entry;
            while ((entry = readdir(dir)) && *count < MAX_MATCHES) {
                if (strncmp(entry->d_name, partial, strlen(partial)) == 0) {
                    // Check if the file is executable
                    char filepath[1024];
                    snprintf(filepath, sizeof(filepath), "%s/%s", path_token, entry->d_name);
                    
                    struct stat st;
                    if (stat(filepath, &st) == 0 && (st.st_mode & S_IXUSR)) {
                        matches[*count] = strdup(entry->d_name);
                        (*count)++;
                    }
                }
            }
            closedir(dir);
        }
        path_token = strtok(NULL, ":");
    }
    
    free(path_copy);
    return matches;
}

// Get completions for a file or directory
static char **get_file_completions(const char *partial, int *count) {
    char **matches = malloc(MAX_MATCHES * sizeof(char *));
    if (!matches) return NULL;
    
    *count = 0;
    
    // Split the partial path into directory and filename part
    char *dir_part = ".";
    char *file_part = strdup(partial);
    
    char *last_slash = strrchr(file_part, '/');
    if (last_slash) {
        *last_slash = '\0';
        dir_part = file_part;
        file_part = last_slash + 1;
    }
    
    // Open the directory
    DIR *dir = opendir(dir_part);
    if (!dir) {
        free(file_part);
        return matches;
    }
    
    // Read directory entries
    struct dirent *entry;
    while ((entry = readdir(dir)) && *count < MAX_MATCHES) {
        if (strncmp(entry->d_name, file_part, strlen(file_part)) == 0) {
            // Create the full path
            char full_path[1024];
            if (strcmp(dir_part, ".") == 0) {
                snprintf(full_path, sizeof(full_path), "%s", entry->d_name);
            } else {
                snprintf(full_path, sizeof(full_path), "%s/%s", dir_part, entry->d_name);
            }
            
            // Add a slash if it's a directory
            if (is_directory(full_path)) {
                char *match = malloc(strlen(entry->d_name) + 2);
                sprintf(match, "%s/", entry->d_name);
                matches[*count] = match;
            } else {
                matches[*count] = strdup(entry->d_name);
            }
            
            (*count)++;
        }
    }
    
    closedir(dir);
    free(file_part);
    
    return matches;
}

// Get tab completion suggestions for a partial input
char **get_completion_matches(const char *partial_input, int *match_count) {
    if (!partial_input || !match_count) return NULL;
    
    *match_count = 0;
    
    // Check if we're completing a command or a file
    if (strchr(partial_input, ' ') == NULL) {
        // Completing a command
        return get_command_completions(partial_input, match_count);
    } else {
        // Completing a file or directory
        char *word = find_current_word(partial_input, strlen(partial_input));
        char **matches = get_file_completions(word, match_count);
        free(word);
        return matches;
    }
}

// Free completion matches
void free_completion_matches(char **matches, int match_count) {
    if (!matches) return;
    
    for (int i = 0; i < match_count; i++) {
        free(matches[i]);
    }
    
    free(matches);
}

// Display tab completion suggestions
void display_completion_matches(char **matches, int match_count) {
    if (!matches || match_count == 0) return;
    
    printf("\n");
    
    for (int i = 0; i < match_count; i++) {
        printf("%s  ", matches[i]);
        if ((i + 1) % 5 == 0) {
            printf("\n");
        }
    }
    
    if (match_count % 5 != 0) {
        printf("\n");
    }
    
    printf("mysh> ");
    fflush(stdout);
}

// Check if the input needs tab completion
int check_completion(char *line, int cursor_pos) {
    // Fix: Add these lines to silence the unused parameter warnings
    (void)line;
    (void)cursor_pos;
    
    // TODO: Implement this if integrating with readline or a similar library
    return 0;
}

// Clean up tab completion resources
void completion_cleanup(void) {
    // Nothing to clean up for basic completion
}