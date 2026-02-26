#ifndef SLOG_DETAILS_FILESYSTEM_HPP
#define SLOG_DETAILS_FILESYSTEM_HPP

#include <cstdio>

#include <string_view>

namespace slog::details
{

SLOG_ALWAYS_INLINE std::FILE* fopen(std::string_view filename, std::string_view mode) noexcept
{
#ifdef _WIN32
    return ::_fsopen(filename.data(), mode.data(), _SH_DENYNO);
#else
    return std::fopen(filename.data(), mode.data());
#endif
}

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

inline std::string_view path_basename(std::string_view path)
{
    std::string_view::size_type pos{std::string_view::npos};

    if (path.empty()) {
        return {};
    }
#ifdef _WIN32
    pos = path.find_last_of("\\");
#else
    pos = path.find_last_of('/');
#endif
    return (pos == std::string_view::npos) ? path : path.substr(pos + 1);
}

}

#endif // SLOG_DETAILS_FILESYSTEM_HPP