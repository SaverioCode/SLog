#ifndef SLOG_DETAILS_MACROS_HPP
#define SLOG_DETAILS_MACROS_HPP

// ----------------------------------------
// Export/Import macros
// ----------------------------------------

#if defined(_WIN32) || defined(__CYGWIN__)
    #if defined(SLOG_SHARED_LIB)
        #if defined(SLOG_LIB_BUILD)
            #define SLOG_API __declspec(dllexport)
        #else
            #define SLOG_API __declspec(dllimport)
        #endif
    #else
        #define SLOG_API
    #endif
#elif defined(__GNUC__) || defined(__clang__)
    #define SLOG_API __attribute__((visibility("default")))
#else
    #define SLOG_API
#endif

// ----------------------------------------
// Inline and Always inline
// ----------------------------------------

#if defined(_MSC_VER)
    #define SLOG_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define SLOG_ALWAYS_INLINE inline __attribute__((always_inline))
#else
    #define SLOG_ALWAYS_INLINE inline
#endif

#if defined(SLOG_LIB_BUILD)
    #define SLOG_INLINE
#else
    #define SLOG_INLINE inline
#endif

// ----------------------------------------
// Temporary disable padding warning for MSVC
// ----------------------------------------

#ifdef _MSC_VER
  #define SLOG_DISABLE_PADDING_WARNING __pragma(warning(push)) __pragma(warning(disable: 4324))
  #define SLOG_RESTORE_PADDING_WARNING __pragma(warning(pop))
#else
  #define SLOG_DISABLE_PADDING_WARNING
  #define SLOG_RESTORE_PADDING_WARNING
#endif

#endif // SLOG_DETAILS_MACROS_HPP