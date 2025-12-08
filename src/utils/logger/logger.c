#include "logger.h"
#include <stdarg.h>
#include <stdio.h>

// Set your default level here. E.g., INFO shows errors, warnings, and general info.
LogLevel g_compiler_log_level = LOG_LEVEL_DEBUG;

void log_message(LogLevel level, const char* file, int line, const char* func, const char* fmt,
                 ...) {
    if (level > g_compiler_log_level) {
        return;
    }

    // Determine the level string
    const char* level_str;
    switch (level) {
    case LOG_LEVEL_ERROR:
        level_str = "ERROR";
        break;
    case LOG_LEVEL_WARN:
        level_str = "WARN ";
        break;
    case LOG_LEVEL_INFO:
        level_str = "INFO ";
        break;
    case LOG_LEVEL_DEBUG:
        level_str = "DEBUG";
        break;
    case LOG_LEVEL_TRACE:
        level_str = "TRACE";
        break;
    default:
        return;
    }

    // Print Context to stderr (Standard Error)
    fprintf(stderr, "[%s] %s:%d %s() - ", level_str, file, line, func);

    // Print the actual log message using vfprintf
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}
