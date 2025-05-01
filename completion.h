#ifndef COMPLETION_H
#define COMPLETION_H

// Initialize tab completion
void completion_init(void);

// Get tab completion suggestions for a partial input
char **get_completion_matches(const char *partial_input, int *match_count);

// Free completion matches
void free_completion_matches(char **matches, int match_count);

// Display tab completion suggestions
void display_completion_matches(char **matches, int match_count);

// Check if the input needs tab completion
int check_completion(char *line, int cursor_pos);

// Clean up tab completion resources
void completion_cleanup(void);

#endif // COMPLETION_H