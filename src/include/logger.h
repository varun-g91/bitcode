#define LOG_ERROR(fmt, ...)                                                                        \
    log_message(LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)                                                                         \
    log_message(LOG_LEVEL_WARN, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)                                                                         \
    log_message(LOG_LEVEL_INFO, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

// Optional: Use conditional compilation to eliminate high-volume debug logs in production
#ifdef COMPILER_DEBUG_BUILD
#define LOG_DEBUG(fmt, ...)                                                                        \
    log_message(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt, ...)                                                                        \
    log_message(LOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...) (void)0
#define LOG_TRACE(fmt, ...) (void)0
#endif

typedef enum {
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE
} LogLevel;

extern LogLevel g_compiler_log_level;

void log_message(LogLevel level, const char* file, int line, const char* func, const char* fmt,
                 ...);
