#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
#include "env.h"

// Head of the linked list of configuration entries
static struct config_entry *config_list = NULL;

// Initialize configuration system
void config_init(void) {
    // Defaults
    config_set("prompt", "mysh> ");
    config_set("history_size", "100");
    config_set("history_file", ".mysh_history");
    
    // Try to load from default config file
    char *home = env_get("HOME");
    if (home) {
        char config_path[1024];
        snprintf(config_path, sizeof(config_path), "%s/.myshrc", home);
        config_load(config_path);
    }
}

// Create a new config entry
static struct config_entry *create_entry(const char *key, const char *value) {
    struct config_entry *entry = malloc(sizeof(struct config_entry));
    if (!entry) return NULL;
    
    entry->key = strdup(key);
    entry->value = strdup(value);
    entry->next = NULL;
    
    return entry;
}

// Load configuration from file
int config_load(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        return -1;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = '\0';
        
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\0') {
            continue;
        }
        
        // Find the first '=' character
        char *equals = strchr(line, '=');
        if (!equals) {
            continue;
        }
        
        // Split the line at the '=' character
        *equals = '\0';
        char *key = line;
        char *value = equals + 1;
        
        // Trim whitespace
        while (*key && (*key == ' ' || *key == '\t')) key++;
        char *end = key + strlen(key) - 1;
        while (end > key && (*end == ' ' || *end == '\t')) {
            *end = '\0';
            end--;
        }
        
        while (*value && (*value == ' ' || *value == '\t')) value++;
        end = value + strlen(value) - 1;
        while (end > value && (*end == ' ' || *end == '\t')) {
            *end = '\0';
            end--;
        }
        
        // Set the config value
        config_set(key, value);
    }
    
    fclose(file);
    return 0;
}

// Get a configuration value
char *config_get(const char *key) {
    struct config_entry *entry = config_list;
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return NULL;
}

// Set a configuration value
int config_set(const char *key, const char *value) {
    // Check if key already exists
    struct config_entry *entry = config_list;
    struct config_entry *prev = NULL;
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            // Update existing entry
            free(entry->value);
            entry->value = strdup(value);
            return 0;
        }
        prev = entry;
        entry = entry->next;
    }
    
    // Add new entry
    entry = create_entry(key, value);
    if (!entry) {
        return -1;
    }
    
    if (!config_list) {
        config_list = entry;
    } else {
        prev->next = entry;
    }
    
    return 0;
}

// Save configuration to file
int config_save(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        return -1;
    }
    
    fprintf(file, "# mysh configuration file\n");
    
    struct config_entry *entry = config_list;
    while (entry) {
        fprintf(file, "%s=%s\n", entry->key, entry->value);
        entry = entry->next;
    }
    
    fclose(file);
    return 0;
}

// Free all configuration resources
void config_cleanup(void) {
    struct config_entry *entry = config_list;
    
    while (entry) {
        struct config_entry *next = entry->next;
        free(entry->key);
        free(entry->value);
        free(entry);
        entry = next;
    }
    
    config_list = NULL;
}