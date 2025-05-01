#ifndef ENV_H
#define ENV_H

// Initialize environment variables manager
void env_init(void);

// Get environment variable value
char *env_get(const char *name);

// Set environment variable value
int env_set(const char *name, const char *value);

// Unset environment variable
int env_unset(const char *name);

// Print all environment variables
void env_print(void);

// Expand environment variables in a string (e.g., $HOME, $PATH)
char *env_expand(const char *str);

// Free resources used by environment variables manager
void env_cleanup(void);

#endif // ENV_H