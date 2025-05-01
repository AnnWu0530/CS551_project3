#ifndef ERROR_H
#define ERROR_H

// Error codes
enum error_code {
    ERROR_NONE = 0,
    ERROR_GENERIC,
    ERROR_MEMORY,
    ERROR_IO,
    ERROR_PARSER,
    ERROR_SYSTEM,
    ERROR_COMMAND_NOT_FOUND,
    ERROR_PERMISSION_DENIED,
    ERROR_NOT_FOUND
};

// Log levels
enum log_level {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
};

// Set the current log level
void error_set_log_level(enum log_level level);

// Log a message with the given level
void error_log(enum log_level level, const char *format, ...);

// Report an error with the given code
void error_report(enum error_code code, const char *format, ...);

// Get the last error code
enum error_code error_last(void);

// Clear the last error
void error_clear(void);

// Get a string description for an error code
const char *error_str(enum error_code code);

// Enable or disable colored output
void error_set_color(int enable);

#endif // ERROR_H