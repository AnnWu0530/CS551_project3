#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "error.h"

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"

// Current log level
static enum log_level current_log_level = LOG_INFO;

// Last error code
static enum error_code last_error = ERROR_NONE;

// Enable/disable colored output
static int use_color = 1;

// Set the current log level
void error_set_log_level(enum log_level level) {
    current_log_level = level;
}

// Get the color for a log level
static const char *get_level_color(enum log_level level) {
    if (!use_color) return "";
    
    switch (level) {
        case LOG_DEBUG:   return COLOR_CYAN;
        case LOG_INFO:    return COLOR_GREEN;
        case LOG_WARNING: return COLOR_YELLOW;
        case LOG_ERROR:   return COLOR_RED;
        case LOG_FATAL:   return COLOR_MAGENTA;
        default:          return COLOR_RESET;
    }
}

// Get the string representation of a log level
static const char *get_level_str(enum log_level level) {
    switch (level) {
        case LOG_DEBUG:   return "DEBUG";
        case LOG_INFO:    return "INFO";
        case LOG_WARNING: return "WARNING";
        case LOG_ERROR:   return "ERROR";
        case LOG_FATAL:   return "FATAL";
        default:          return "UNKNOWN";
    }
}

// Log a message with the given level
void error_log(enum log_level level, const char *format, ...) {
    if (level < current_log_level) {
        return;  // Skip logs below the current level
    }
    
    FILE *out = (level >= LOG_WARNING) ? stderr : stdout;
    
    // Get current time
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    // Print log header
    fprintf(out, "%s [%s%s%s] ", 
            time_str, 
            get_level_color(level), 
            get_level_str(level), 
            use_color ? COLOR_RESET : "");
    
    // Print log message
    va_list args;
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);
    
    fprintf(out, "\n");
    fflush(out);
    
    // Exit on fatal errors
    if (level == LOG_FATAL) {
        exit(EXIT_FAILURE);
    }
}

// Report an error with the given code
void error_report(enum error_code code, const char *format, ...) {
    last_error = code;
    
    // Convert error code to log level
    enum log_level level;
    switch (code) {
        case ERROR_NONE:
            level = LOG_INFO;
            break;
        case ERROR_GENERIC:
        case ERROR_MEMORY:
        case ERROR_IO:
        case ERROR_PARSER:
        case ERROR_SYSTEM:
            level = LOG_ERROR;
            break;
        case ERROR_COMMAND_NOT_FOUND:
        case ERROR_PERMISSION_DENIED:
        case ERROR_NOT_FOUND:
            level = LOG_WARNING;
            break;
        default:
            level = LOG_ERROR;
    }
    
    // Print error message
    va_list args;
    va_start(args, format);
    
    // Prefix with error code string
    char full_format[1024];
    snprintf(full_format, sizeof(full_format), "%s: %s", 
             error_str(code), format);
    
    // Log the error
    error_log(level, full_format, args);
    
    va_end(args);
}

// Get the last error code
enum error_code error_last(void) {
    return last_error;
}

// Clear the last error
void error_clear(void) {
    last_error = ERROR_NONE;
}

// Get a string description for an error code
const char *error_str(enum error_code code) {
    switch (code) {
        case ERROR_NONE:              return "No error";
        case ERROR_GENERIC:           return "Generic error";
        case ERROR_MEMORY:            return "Memory allocation error";
        case ERROR_IO:                return "I/O error";
        case ERROR_PARSER:            return "Parser error";
        case ERROR_SYSTEM:            return "System error";
        case ERROR_COMMAND_NOT_FOUND: return "Command not found";
        case ERROR_PERMISSION_DENIED: return "Permission denied";
        case ERROR_NOT_FOUND:         return "File or directory not found";
        default:                      return "Unknown error";
    }
}

// Enable or disable colored output
void error_set_color(int enable) {
    use_color = enable;
}