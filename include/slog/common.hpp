#ifndef SLOG_CONFIG_HPP
#define SLOG_CONFIG_HPP

// ----------------------------------------
// Compile-time Max log level
// ----------------------------------------

#ifndef SLOG_MAX_LOG_LEVEL
    #define SLOG_MAX_LOG_LEVEL static_cast<uint8_t>(slog::LogLevel::TRACE)
#endif

// ----------------------------------------
// MPSC Queue Size
// ----------------------------------------

#ifndef SLOG_MPSC_QUEUE_SIZE
    #define SLOG_MPSC_QUEUE_SIZE 8192
#endif

// ----------------------------------------
// Default and Inactive loggers and sinks name
// ----------------------------------------

#define _SLOG_DEFAULT_LOGGER_NAME "default"
#define _SLOG_INACTIVE_LOGGER_NAME "inactive"
#define _SLOG_DEFAULT_SINK_NAME "console_out"
#define _SLOG_INACTIVE_SINK_NAME "console_err"

// ----------------------------------------
// Inline and Always inline
// ----------------------------------------

#if !defined(SLOG_LIB_BUILD)
    #if defined(_MSC_VER)
        #define SLOG_ALWAYS_INLINE __forceinline
    #elif defined(__GNUC__) || defined(__clang__)
        #define SLOG_ALWAYS_INLINE inline __attribute__((always_inline))
    #else
        #define SLOG_ALWAYS_INLINE inline
    #endif
    #define SLOG_INLINE inline
#else
    #define SLOG_INLINE
    #define SLOG_ALWAYS_INLINE
#endif

// ----------------------------------------
// Thread safety
// ----------------------------------------

#if defined(SLOG_TSAFE)
    #include <mutex>

    #define SLOG_MUTEX_MEMBER(name) std::mutex name;
    #define SLOG_LOCK(name) std::lock_guard<std::mutex> lock(name);
#else
    #define SLOG_MUTEX_MEMBER(name)
    #define SLOG_LOCK(name)
#endif

// ----------------------------------------
// Async mode macros
// ----------------------------------------

#if defined(SLOG_ASYNC_ENABLED)
    #define SLOG_SINK_MUTEX_MEMBER(name)
    #define SLOG_SINK_LOCK(name)
#else
    #define SLOG_SINK_MUTEX_MEMBER(name) SLOG_MUTEX_MEMBER(name)
    #define SLOG_SINK_LOCK(name)         SLOG_LOCK(name)
#endif

// ----------------------------------------
// Stream macros
// ----------------------------------------

#ifdef SLOG_STREAM_DISABLED
    #ifdef SLOG_STREAM_ENABLED
        #undef SLOG_STREAM_ENABLED
    #endif
#else
    #define SLOG_STREAM_ENABLED
#endif


#endif // SLOG_CONFIG_HPP