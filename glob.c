#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fnmatch.h>
#include "glob.h"

#define MAX_PATTERN_MATCHES 1024
#define MAX_COMMAND_LENGTH 4096

// Check if a string contains wildcard characters
static int has_wildcards(const char *str) {
    return (strchr(str, '*') != NULL || 
            strchr(str, '?') != NULL || 
            strchr(str, '[') != NULL);
}

// Expand a single wildcard pattern into matching filenames
char *expand_pattern(const char *pattern) {
    DIR *dir;
    struct dirent *entry;
    char *dir_path = ".";
    char *file_pattern = strdup(pattern);
    char *matches[MAX_PATTERN_MATCHES];
    int match_count = 0;
    
    // Extract directory path if present
    char *last_slash = strrchr(file_pattern, '/');
    if (last_slash) {
        *last_slash = '\0';
        dir_path = file_pattern;
        file_pattern = last_slash + 1;
    }
    
    // Open the directory
    dir = opendir(dir_path);
    if (!dir) {
        // Could not open directory, return the original pattern
        if (last_slash) {
            *last_slash = '/';  // Restore the slash
        }
        return strdup(pattern);
    }
    
    // Scan the directory for matches
    while ((entry = readdir(dir)) && match_count < MAX_PATTERN_MATCHES) {
        // Skip . and .. entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Check if the filename matches the pattern
        if (fnmatch(file_pattern, entry->d_name, 0) == 0) {
            // Construct the full path if needed
            if (strcmp(dir_path, ".") == 0) {
                matches[match_count++] = strdup(entry->d_name);
            } else {
                char *full_path = malloc(strlen(dir_path) + strlen(entry->d_name) + 2);
                sprintf(full_path, "%s/%s", dir_path, entry->d_name);
                matches[match_count++] = full_path;
            }
        }
    }
    closedir(dir);
    
    // If no matches found, return the original pattern
    if (match_count == 0) {
        if (last_slash) {
            *last_slash = '/';  // Restore the slash
        }
        free(file_pattern);
        return strdup(pattern);
    }
    
    // Combine all matches into a single space-delimited string
    size_t total_length = 0;
    for (int i = 0; i < match_count; i++) {
        total_length += strlen(matches[i]) + 1;  // +1 for space or null
    }
    
    char *result = malloc(total_length);
    result[0] = '\0';
    
    for (int i = 0; i < match_count; i++) {
        strcat(result, matches[i]);
        if (i < match_count - 1) {
            strcat(result, " ");
        }
        free(matches[i]);
    }
    
    free(file_pattern);
    return result;
}

// Check if a filename matches a pattern
int match_pattern(const char *pattern, const char *filename) {
    return fnmatch(pattern, filename, 0) == 0;
}

// Expand wildcards in a command line
char *expand_wildcards(const char *command_line) {
    if (!command_line || !has_wildcards(command_line)) {
        return strdup(command_line);  // No wildcards to expand
    }
    
    // Tokenize the command line
    char *cmd_copy = strdup(command_line);
    char *expanded_cmd = malloc(MAX_COMMAND_LENGTH);
    expanded_cmd[0] = '\0';
    
    char *token = strtok(cmd_copy, " \t");
    while (token) {
        // Check if token has wildcards
        if (has_wildcards(token)) {
            char *expansion = expand_pattern(token);
            if (expanded_cmd[0] != '\0') {
                strcat(expanded_cmd, " ");
            }
            strcat(expanded_cmd, expansion);
            free(expansion);
        } else {
            // No wildcards, just append the token
            if (expanded_cmd[0] != '\0') {
                strcat(expanded_cmd, " ");
            }
            strcat(expanded_cmd, token);
        }
        
        token = strtok(NULL, " \t");
    }
    
    free(cmd_copy);
    return expanded_cmd;
}