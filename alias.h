#ifndef ALIAS_H
#define ALIAS_H

// Initialize alias system
void alias_init(void);

// Define an alias
int alias_define(const char *name, const char *value);

// Remove an alias
int alias_remove(const char *name);

// Get the command for an alias
// Returns NULL if no alias found
char *alias_get(const char *name);

// List all defined aliases
void alias_list(void);

// Expand aliases in a command line
// Returns a newly allocated string with aliases expanded
char *alias_expand(const char *command_line);

// Free all alias resources
void alias_cleanup(void);

#endif // ALIAS_H