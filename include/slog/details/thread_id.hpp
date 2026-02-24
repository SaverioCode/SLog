#ifndef SLOG_DETAILS_THREAD_ID_HPP
#define SLOG_DETAILS_THREAD_ID_HPP

#include <cstddef>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #define NOGDI
    #define NOMINMAX
    #define NOSERVICE
    #define NOMCX
    #define NOIME
    #define NOCOMM
    #define NOKANJI
    #define NOCRYPT
    #define NOHELP
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
#elif defined(__APPLE__)
    #include <pthread.h>
#else
    #include <thread>
#endif

namespace slog::details
{

// Returns the native OS thread ID for the calling thread
// Fallback: hash std::this_thread::get_id()
inline size_t current_thread_id() noexcept
{
#ifdef _WIN32
    return static_cast<size_t>(::GetCurrentThreadId());
#elif defined(__linux__)
    return static_cast<size_t>(::gettid());
#elif defined(__APPLE__)
    uint64_t tid = 0;
    ::pthread_threadid_np(nullptr, &tid);
    return static_cast<size_t>(tid);
#else
    return std::hash<std::thread::id>{}(std::this_thread::get_id());
#endif
}

} // namespace slog::details

#endif // SLOG_DETAILS_THREAD_ID_HPP
