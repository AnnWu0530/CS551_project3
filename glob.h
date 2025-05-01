#ifndef GLOB_H
#define GLOB_H

// Expand wildcards in a command line
// For example, "ls *.c" becomes "ls file1.c file2.c file3.c"
char *expand_wildcards(const char *command_line);

// Expand a single wildcard pattern into matching filenames
// Returns a newly allocated string with the expansion
char *expand_pattern(const char *pattern);

// Check if a filename matches a pattern
int match_pattern(const char *pattern, const char *filename);

#endif // GLOB_H