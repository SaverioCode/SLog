#ifndef SLOG_CORE_LOG_RECORD_HPP
#define SLOG_CORE_LOG_RECORD_HPP

#include <any>
#include <chrono>
#include <iosfwd>
#include <source_location>
#include <string_view>

#include <slog/core/log_level.hpp>

namespace slog
{

struct LogRecord
{
    LogLevel level;
    std::string_view logger_name;
    std::string message; // pattern formatted message
    std::string string_buffer; // raw buffer
    std::source_location location;
    std::chrono::system_clock::time_point timestamp;
    size_t thread_id;
    std::any stored_args;
    std::string_view format_str;
    void (*format_fn)(std::string_view, std::any&, std::string&){nullptr};

    LogRecord() : level(LogLevel::INFO) {}
    LogRecord(LogRecord&&) noexcept = default;
    LogRecord(const LogRecord&) = default;
    ~LogRecord() = default;

    LogRecord& operator=(LogRecord&&) noexcept = default;
    LogRecord& operator=(const LogRecord&) = default;
};

} // namespace slog

#endif // SLOG_CORE_LOG_RECORD_HPP
