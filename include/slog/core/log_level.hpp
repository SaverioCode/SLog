#ifndef SLOG_CORE_LOG_LEVEL_HPP
#define SLOG_CORE_LOG_LEVEL_HPP

#include <cstdint>
#include <string_view>

namespace slog
{

enum class LogLevel : uint8_t
{
    FATAL = 1,
    ERROR = 2,
    WARNING = 3,
    INFO = 4,
    DEBUG = 5,
    TRACE = 6
};

constexpr std::string_view to_string(LogLevel level) noexcept
{
    switch (level) {
        case LogLevel::FATAL:  return "FATAL";
        case LogLevel::ERROR:  return "ERROR";
        case LogLevel::WARNING:return "WARNING";
        case LogLevel::INFO:   return "INFO";
        case LogLevel::DEBUG:  return "DEBUG";
        case LogLevel::TRACE:  return "TRACE";
        default:               return "UNKNOWN";
    }
}

}

#endif // SLOG_CORE_LOG_LEVEL_HPP