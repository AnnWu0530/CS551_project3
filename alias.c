#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "alias.h"
#include "config.h"

// Alias structure
struct alias_entry {
    char *name;
    char *value;
    struct alias_entry *next;
};

// Head of the linked list of aliases
static struct alias_entry *alias_head = NULL;

// Forward declaration of helper function
static void save_aliases_to_config(void);

// Initialize alias system
void alias_init(void) {
    // Set up some default aliases
    alias_define("ll", "ls -la");
    alias_define("la", "ls -a");
    alias_define("l", "ls -l");
    alias_define("cls", "clear");
    
    // Load aliases from config if available
    char *aliases = config_get("aliases");
    if (aliases) {
        char *aliases_copy = strdup(aliases);
        char *token = strtok(aliases_copy, ";");
        
        while (token) {
            char *equals = strchr(token, '=');
            if (equals) {
                *equals = '\0';
                alias_define(token, equals + 1);
            }
            token = strtok(NULL, ";");
        }
        
        free(aliases_copy);
    }
}

// Define an alias
int alias_define(const char *name, const char *value) {
    if (!name || !value) return -1;
    
    // Check if alias already exists
    struct alias_entry *entry = alias_head;
    
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            // Update existing alias
            free(entry->value);
            entry->value = strdup(value);
            
            // Update aliases in config
            save_aliases_to_config();
            
            return 0;
        }
        entry = entry->next;
    }
    
    // Create new alias
    entry = malloc(sizeof(struct alias_entry));
    if (!entry) return -1;
    
    entry->name = strdup(name);
    entry->value = strdup(value);
    entry->next = alias_head;
    alias_head = entry;
    
    // Update aliases in config
    save_aliases_to_config();
    
    return 0;
}

// Save all aliases to config
static void save_aliases_to_config(void) {
    struct alias_entry *entry = alias_head;
    char aliases[4096] = "";
    
    while (entry) {
        char alias_str[1024];
        snprintf(alias_str, sizeof(alias_str), "%s=%s;", entry->name, entry->value);
        
        if (strlen(aliases) + strlen(alias_str) < sizeof(aliases)) {
            strcat(aliases, alias_str);
        }
        
        entry = entry->next;
    }
    
    config_set("aliases", aliases);
}

// Remove an alias
int alias_remove(const char *name) {
    if (!name) return -1;
    
    struct alias_entry *entry = alias_head;
    struct alias_entry *prev = NULL;
    
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            // Remove this entry
            if (prev) {
                prev->next = entry->next;
            } else {
                alias_head = entry->next;
            }
            
            free(entry->name);
            free(entry->value);
            free(entry);
            
            // Update aliases in config
            save_aliases_to_config();
            
            return 0;
        }
        
        prev = entry;
        entry = entry->next;
    }
    
    return -1;  // Alias not found
}

// Get the command for an alias
char *alias_get(const char *name) {
    if (!name) return NULL;
    
    struct alias_entry *entry = alias_head;
    
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return NULL;  // Alias not found
}

// List all defined aliases
void alias_list(void) {
    struct alias_entry *entry = alias_head;
    
    while (entry) {
        printf("alias %s='%s'\n", entry->name, entry->value);
        entry = entry->next;
    }
}

// Expand aliases in a command line
char *alias_expand(const char *command_line) {
    if (!command_line) return NULL;
    
    // Make a copy of the command line
    char *expanded = strdup(command_line);
    if (!expanded) return NULL;
    
    // Get the first word (command)
    char command[256] = "";
    char *first_space = strchr(expanded, ' ');
    
    if (first_space) {
        strncpy(command, expanded, first_space - expanded);
        command[first_space - expanded] = '\0';
    } else {
        strcpy(command, expanded);
    }
    
    // Look up the command in the alias list
    char *alias_value = alias_get(command);
    if (!alias_value) {
        return expanded;  // No alias found
    }
    
    // Replace the command with its alias
    char *new_command = malloc(strlen(alias_value) + (first_space ? strlen(first_space) : 0) + 1);
    if (!new_command) {
        free(expanded);
        return NULL;
    }
    
    strcpy(new_command, alias_value);
    
    // Append the rest of the original command if any
    if (first_space) {
        strcat(new_command, first_space);
    }
    
    free(expanded);
    
    // Check for nested aliases (aliases that contain other aliases)
    // This prevents infinite recursion
    char *nested_expansion = NULL;
    char *first_word = strtok(strdup(new_command), " ");
    if (first_word && strcmp(first_word, command) != 0) {
        char *tmp = strdup(new_command);
        nested_expansion = alias_expand(tmp);
        free(tmp);
        free(first_word);
        free(new_command);
        return nested_expansion;
    }
    
    free(first_word);
    return new_command;
}

// Free all alias resources
void alias_cleanup(void) {
    struct alias_entry *entry = alias_head;
    
    while (entry) {
        struct alias_entry *next = entry->next;
        free(entry->name);
        free(entry->value);
        free(entry);
        entry = next;
    }
    
    alias_head = NULL;
}