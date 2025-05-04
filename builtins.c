#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "builtins.h"
#include "env.h"

// Define builtin commands
typedef struct {
    const char *name;
    int (*func)(char **args);
    const char *help;
} builtin_t;

// Array of built-in commands
static builtin_t builtins[] = {
    {"cd", builtin_cd, "Change the current directory"},
    {"pwd", builtin_pwd, "Print the current working directory"},
    {"echo", builtin_echo, "Print arguments to stdout"},
    {"export", builtin_export, "Set environment variables"},
    {"unset", builtin_unset, "Unset environment variables"},
    {"exit", builtin_exit, "Exit the shell"},
    {"help", builtin_help, "Display help about built-in commands"},
    {NULL, NULL, NULL}
};

static builtin_t others[] = {
    {"alias", builtin_cd, "Change the current directory"},
    {"jobs", builtin_pwd, "List active jobs"},
    {"fg", builtin_echo, "Resume job in foreground"},
    {"bg", builtin_export, "Resume job in background"},
    {"kill", builtin_unset, "Send signal to jobs/processes"},
    {"history", builtin_exit, "Display command history"},
    {NULL, NULL, NULL},
    {NULL, NULL, NULL}
};

// Execute a built-in command if the command matches one
int execute_builtin(struct command *cmd) {
    if (!cmd || !cmd->argv[0]) return 0;
    
    for (int i = 0; builtins[i].name != NULL; i++) {
        if (strcmp(cmd->argv[0], builtins[i].name) == 0) {
            return builtins[i].func(cmd->argv);
        }
    }
    
    return 0; // Not a built-in command
}

// cd - Change directory
int builtin_cd(char **args) {
    if (!args[1]) {
        // No argument provided, change to HOME directory
        char *home = env_get("HOME");
        if (!home) {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
        if (chdir(home) != 0) {
            perror("cd");
            return 1;
        }
    } else {
        if (chdir(args[1]) != 0) {
            perror("cd");
            return 1;
        }
    }
    
    // Update PWD environment variable
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        env_set("PWD", cwd);
        printf("Now directort:%s\n", cwd);
    } else {
        perror("getcwd");
    }
    
    return 1;
}

// pwd - Print working directory
int builtin_pwd(char **args) {
    // Fix: Add this line to silence the unused parameter warning
    (void)args;
    
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
        return 1;
    }
    return 1;
}

// echo - Print arguments
int builtin_echo(char **args) {
    int i = 1;
    
    // Check for -n option (no newline)
    int print_newline = 1;
    if (args[1] && strcmp(args[1], "-n") == 0) {
        print_newline = 0;
        i = 2;
    }
    
    while (args[i]) {
        // Expand environment variables
        char *expanded = env_expand(args[i]);
        if (expanded) {
            printf("%s", expanded);
            free(expanded);
        } else {
            printf("%s", args[i]);
        }
        
        if (args[i+1]) {
            printf(" ");
        }
        i++;
    }
    
    if (print_newline) {
        printf("\n");
    }
    
    return 1;
}

// export - Set environment variables
int builtin_export(char **args) {
    if (!args[1]) {
        // With no arguments, print all environment variables
        env_print();
        return 1;
    }
    
    for (int i = 1; args[i]; i++) {
        char *eq = strchr(args[i], '=');
        if (eq) {
            // Split at the '=' character
            *eq = '\0';
            char *name = args[i];
            char *value = eq + 1;
            
            if (env_set(name, value) != 0) {
                fprintf(stderr, "export: failed to set %s\n", name);
                return 1;
            }
            
            // Restore the '='
            *eq = '=';
        } else {
            fprintf(stderr, "export: invalid format: %s\n", args[i]);
            return 1;
        }
    }
    
    return 1;
}

// unset - Unset environment variables
int builtin_unset(char **args) {
    if (!args[1]) {
        fprintf(stderr, "unset: missing arguments\n");
        return 1;
    }
    
    for (int i = 1; args[i]; i++) {
        if (env_unset(args[i]) != 0) {
            fprintf(stderr, "unset: failed to unset %s\n", args[i]);
            return 1;
        }
    }
    
    return 1;
}

// exit - Exit the shell
int builtin_exit(char **args) {
    int status = 0;
    
    if (args[1]) {
        status = atoi(args[1]);
    }
    
    printf("Goodbye!\n");
    exit(status);
    
    return 1; // Never reached
}

// help - Display help about built-in commands
int builtin_help(char **args) {
    if (!args[1]) {
        // If no command is specified, list all built-in commands
        printf("Built-in commands:\n");
        for (int i = 0; builtins[i].name != NULL; i++) {
            printf("  %-10s %s\n", builtins[i].name, builtins[i].help);
        }
        for (int i = 0; others[i].name != NULL; i++) {
            printf("  %-10s %s\n", others[i].name, others[i].help);
        }
    } else {
        // Display help for the specified command
        for (int i = 0; builtins[i].name != NULL; i++) {
            if (strcmp(args[1], builtins[i].name) == 0) {
                printf("%s: %s\n", builtins[i].name, builtins[i].help);
                return 1;
            }
        }
        for (int i = 0; others[i].name != NULL; i++) {
            if (strcmp(args[1], others[i].name) == 0) {
                printf("%s: %s\n", others[i].name, others[i].help);
                return 1;
            }
        }
        fprintf(stderr, "help: no help for %s\n", args[1]);
        return 1;
    }
    
    return 1;
}
