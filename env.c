#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "env.h"

extern char **environ;

// Initialize environment variables manager
void env_init(void) {
    // No special initialization needed as we use the system's environ
}

// Get environment variable value
char *env_get(const char *name) {
    return getenv(name);
}

// Set environment variable value
int env_set(const char *name, const char *value) {
    if (!name || !value) return -1;
    
    char *env_var = malloc(strlen(name) + strlen(value) + 2); // +2 for '=' and null terminator
    if (!env_var) return -1;
    
    sprintf(env_var, "%s=%s", name, value);
    return putenv(env_var);
}

// Unset environment variable
int env_unset(const char *name) {
    if (!name) return -1;
    return unsetenv(name);
}

// Print all environment variables
void env_print(void) {
    for (char **env = environ; *env != NULL; env++) {
        printf("%s\n", *env);
    }
}

// Helper function to check if a character is valid in a variable name
static int is_valid_var_char(char c, int pos) {
    return (isalnum(c) || c == '_') && (pos > 0 || !isdigit(c));
}

// Expand environment variables in a string (e.g., $HOME, $PATH)
char *env_expand(const char *str) {
    if (!str) return NULL;
    
    // Count the expanded size
    size_t expanded_size = 0;
    const char *p = str;
    
    while (*p) {
        if (*p == '$' && *(p+1) != '\0' && (isalpha(*(p+1)) || *(p+1) == '_')) {
            // Environment variable found
            const char *var_start = p + 1;
            const char *var_end = var_start;
            
            while (*var_end && is_valid_var_char(*var_end, var_end - var_start)) {
                var_end++;
            }
            
            size_t var_name_len = var_end - var_start;
            char var_name[var_name_len + 1];
            strncpy(var_name, var_start, var_name_len);
            var_name[var_name_len] = '\0';
            
            char *var_value = getenv(var_name);
            if (var_value) {
                expanded_size += strlen(var_value);
            }
            
            p = var_end;
        } else {
            expanded_size++;
            p++;
        }
    }
    
    // Allocate the expanded string
    char *expanded = malloc(expanded_size + 1);
    if (!expanded) return NULL;
    
    // Fill the expanded string
    char *dest = expanded;
    p = str;
    
    while (*p) {
        if (*p == '$' && *(p+1) != '\0' && (isalpha(*(p+1)) || *(p+1) == '_')) {
            // Environment variable found
            const char *var_start = p + 1;
            const char *var_end = var_start;
            
            while (*var_end && is_valid_var_char(*var_end, var_end - var_start)) {
                var_end++;
            }
            
            size_t var_name_len = var_end - var_start;
            char var_name[var_name_len + 1];
            strncpy(var_name, var_start, var_name_len);
            var_name[var_name_len] = '\0';
            
            char *var_value = getenv(var_name);
            if (var_value) {
                strcpy(dest, var_value);
                dest += strlen(var_value);
            }
            
            p = var_end;
        } else {
            *dest++ = *p++;
        }
    }
    
    *dest = '\0';
    return expanded;
}

// Free resources used by environment variables manager
void env_cleanup(void) {
    // No special cleanup needed for this implementation
}