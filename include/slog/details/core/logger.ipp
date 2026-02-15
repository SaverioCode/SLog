#ifndef SLOG_CORE_LOGGER_IPP
#define SLOG_CORE_LOGGER_IPP

#include <chrono> // Todo: remove this when cleaned get_time_stamp

#include <slog/core/log_record.hpp>
#include <slog/core/logger.hpp>
#include <slog/sinks/sink_manager.hpp>

namespace slog
{

// ------------------------
// Public methods
// ------------------------

SLOG_ALWAYS_INLINE void    Logger::addSink(std::shared_ptr<slog::sinks::ISink> sink)
{
    _sink_manager.addSink(sink);
}

SLOG_ALWAYS_INLINE void    Logger::removeSink(const std::string& name) noexcept
{
    _sink_manager.removeSink(name);
}

// ------------------------
// Private methods
// ------------------------

SLOG_INLINE Logger::Logger(std::string_view name) : _name(name) {}

SLOG_INLINE Logger::Logger(const std::shared_ptr<slog::sinks::ISink> sink) : 
    _sink_manager(sink), _log_level(LogLevel::TRACE) {}

SLOG_INLINE Logger::Logger(const std::vector<std::shared_ptr<slog::sinks::ISink>> sinks) : 
    _sink_manager(sinks), _log_level(LogLevel::TRACE) {}

SLOG_ALWAYS_INLINE void    Logger::_submit(const LogLevel level, std::string&& message, std::source_location loc)
{
    LogRecord record{level, std::move(message), loc};

    _sink_manager.dispatch(record);
}

SLOG_INLINE std::string  Logger::_getTimeStamp()
{
    char formatted[20];

    std::time_t timestamp = std::time(nullptr);
    std::tm* datetime = std::localtime(&timestamp);
    std::strftime(formatted, sizeof(formatted), "%Y-%m-%d %H:%M:%S", datetime);
    return std::string(formatted);
}
}

#endif // SLOG_CORE_LOGGER_IPP
