// history.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "history.h"

char *history[HISTORY_SIZE];  // Array to store command history
int history_count = 0;        // Tracks the number of commands in history

// Adds a command to the history
void add_to_history(const char *command) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(command);  // Duplicate the command and store it
    } else {
        // Free the memory of the oldest command and shift all others down
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++) {
            history[i - 1] = history[i];
        }
        // Add the new command at the end
        history[HISTORY_SIZE - 1] = strdup(command);
    }
}

// Displays the history of commands
void display_history() {
    if (history_count == 0) {
        printf("No history available.\n");
        return;
    }
    for (int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

// Runs a command from the history by index (zero-indexed)
void run_from_history(int index) {
    if (index >= 0 && index < history_count) {
        printf("Running command from history: %s\n", history[index]);
        system(history[index]);  // Execute the command (using system or execvp)
    } else {
        printf("No such history command: %d\n", index + 1);
    }
}

// Load history from a file
void load_history() {
    FILE *history_file = fopen(".mysh_history", "r");
    if (history_file) {
        char line[1024];
        while (fgets(line, sizeof(line), history_file)) {
            line[strcspn(line, "\n")] = '\0';  // Remove newline character
            add_to_history(line);  // Add the line to the history
        }
        fclose(history_file);
    }
}

// Save history to a file
void save_history() {
    printf("hello");
    FILE *history_file = fopen(".mysh_history", "w");
    if (history_file) {
        for (int i = 0; i < history_count; i++) {
            fprintf(history_file, "%s\n", history[i]);
        }
        fclose(history_file);
    }
}
