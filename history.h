#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_SIZE 100  // Max number of commands to store in history

void add_to_history(const char *command);
void display_history();
void run_from_history(int index);
void load_history();
void save_history();

extern char *history[HISTORY_SIZE];
extern int history_count;

#endif  
