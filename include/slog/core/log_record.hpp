#ifndef SLOG_CORE_LOG_RECORD_HPP
#define SLOG_CORE_LOG_RECORD_HPP

#include <chrono>
#include <iosfwd>
#include <source_location>
#include <thread>

#include <slog/core/log_level.hpp>

namespace slog
{

struct LogRecord
{
    LogLevel level;
    std::string message;
    std::source_location location;
    std::chrono::system_clock::time_point timestamp;
    std::thread::id thread_id;

    LogRecord() : level(LogLevel::INFO) {}
    LogRecord(LogLevel lvl, std::string&& msg, std::source_location loc,
              std::chrono::system_clock::time_point ts)
        : level(lvl), message(std::move(msg)), location(loc),
          timestamp(ts), thread_id(std::this_thread::get_id())
    {
    }
    LogRecord(LogRecord&&) noexcept = default;
    LogRecord(const LogRecord&) = default;
    ~LogRecord() = default;

    LogRecord& operator=(LogRecord&&) noexcept = default;
    LogRecord& operator=(const LogRecord&) = default;
};

} // namespace slog

#endif // SLOG_CORE_LOG_RECORD_HPP
