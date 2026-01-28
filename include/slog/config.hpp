#ifndef SLOG_CONFIG_HPP
#define SLOG_CONFIG_HPP

// ----------------------------------------
// Default log level
// ----------------------------------------
#ifndef SLOG_MAX_LOG_LEVEL
    #define SLOG_MAX_LOG_LEVEL 6
#endif

// ----------------------------------------
// Force inline
// ----------------------------------------
#if defined(_MSC_VER)
    #define SLOG_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define SLOG_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define SLOG_FORCE_INLINE inline
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