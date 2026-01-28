#include <slog/core/log_record.hpp>
#include <slog/core/logger.hpp>
#include <slog/sinks/sink_manager.hpp>

namespace slog::core
{

// ------------------------
// Public methods
// ------------------------

SLOG_FORCE_INLINE void    Logger::addSink(std::shared_ptr<slog::sinks::ISink> sink)
{
    _sink_manager.addSink(sink);
}

SLOG_FORCE_INLINE void    Logger::removeSink(const std::string& name) noexcept
{
    _sink_manager.removeSink(name);
}

// ------------------------
// Private methods
// ------------------------

inline Logger::Logger(const std::shared_ptr<slog::sinks::ISink> sink) : 
    _sink_manager(sink), _global_log_level(LogLevel::TRACE) {}

inline Logger::Logger(const std::vector<std::shared_ptr<slog::sinks::ISink>> sinks) : 
    _sink_manager(sinks), _global_log_level(LogLevel::TRACE) {}

SLOG_FORCE_INLINE void    Logger::_submit(const LogLevel level, std::string&& message, std::source_location loc)
{
    LogRecord record{level, std::move(message), loc};

    _sink_manager.dispatch(record);
}

}
