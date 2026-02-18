#ifndef SLOG_SINKS_COMMON_HPP
#define SLOG_SINKS_COMMON_HPP

#include <cstdio>

#include <slog/common.hpp>

namespace slog::sinks
{

SLOG_ALWAYS_INLINE void fwrite_file(const void* data, const size_t size, std::FILE* stream)
{
#ifdef SLOG_FILE_FWRITE_UNLOCKED
    slog::sinks::_fwrite_unlocked(data, size, stream);
#else
    std::fwrite(data, sizeof(char), size, stream);
#endif
}

SLOG_ALWAYS_INLINE void fwrite_console(const void* data, const size_t size, std::FILE* stream)
{
#ifdef SLOG_CONSOLE_FWRITE_UNLOCKED
    slog::sinks::_fwrite_unlocked(data, size, stream);
#else
    std::fwrite(data, sizeof(char), size, stream);
#endif
}

SLOG_ALWAYS_INLINE void _fwrite_unlocked(const void* data, const size_t size, std::FILE* stream)
{
#ifdef _WIN32
    ::_fwrite_nolock(data, sizeof(char), size, stream);
#else
    ::fwrite_unlocked(data, sizeof(char), size, stream);
#endif
}

} // namespace slog::sinks

#endif // SLOG_SINKS_COMMON_HPP
