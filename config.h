#ifndef CONFIG_H
#define CONFIG_H

// Configuration key-value pair structure
struct config_entry {
    char *key;
    char *value;
    struct config_entry *next;
};

// Initialize configuration system
void config_init(void);

// Load configuration from file
int config_load(const char *filename);

// Get a configuration value
char *config_get(const char *key);

// Set a configuration value
int config_set(const char *key, const char *value);

// Save configuration to file
int config_save(const char *filename);

// Free all configuration resources
void config_cleanup(void);

#endif // CONFIG_H