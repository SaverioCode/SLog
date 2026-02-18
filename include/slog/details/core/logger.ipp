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

SLOG_ALWAYS_INLINE void Logger::flush() const
{
    _sink_manager->flush();
}

SLOG_ALWAYS_INLINE void Logger::add_sink(std::shared_ptr<slog::sinks::ISink> sink)
{
    _sink_manager->add_sink(sink);
}

SLOG_ALWAYS_INLINE void Logger::remove_sink(const std::string& name) noexcept
{
    _sink_manager->remove_sink(name);
}

[[nodiscard]] SLOG_ALWAYS_INLINE std::shared_ptr<slog::sinks::ISink> Logger::get_sink(const std::string& name) const
{
    return _sink_manager->get_sink(name);
}

[[nodiscard]] SLOG_ALWAYS_INLINE std::vector<std::shared_ptr<slog::sinks::ISink>> Logger::get_sinks() const
{
    return _sink_manager->get_sinks();
}

// ------------------------
// Private methods
// ------------------------

SLOG_INLINE Logger::Logger(std::string_view name, std::shared_ptr<Worker> worker)
    : _name(name), _worker(worker)
{
    _sink_manager = std::make_shared<slog::sinks::SinkManager>();
}

SLOG_INLINE Logger::Logger(std::string_view name, const std::shared_ptr<slog::sinks::ISink> sink,
                           std::shared_ptr<Worker> worker)
    : _name(name), _worker(worker), _log_level(LogLevel::TRACE)
{
    _sink_manager = std::make_shared<slog::sinks::SinkManager>(sink);
}

SLOG_INLINE Logger::Logger(std::string_view name,
                           const std::vector<std::shared_ptr<slog::sinks::ISink>> sinks,
                           std::shared_ptr<Worker> worker)
    : _name(name), _worker(worker), _log_level(LogLevel::TRACE)
{
    _sink_manager = std::make_shared<slog::sinks::SinkManger>(sinks);
}

SLOG_ALWAYS_INLINE void Logger::_submit(const LogLevel level, std::string&& message,
                                        std::source_location loc)
{
    LogRecord record{level, std::move(message), loc};

#ifdef SLOG_ASYNC_ENABLED
    _worker->push(AsyncOp{record, _sink_manager});
#else
    _sink_manager->dispatch(record);
#endif
}

SLOG_INLINE std::string Logger::_getTimeStamp()
{
    char formatted[20];

    std::time_t timestamp = std::time(nullptr);
    std::tm* datetime = std::localtime(&timestamp);
    std::strftime(formatted, sizeof(formatted), "%Y-%m-%d %H:%M:%S", datetime);
    return std::string(formatted);
}
} // namespace slog

#endif // SLOG_CORE_LOGGER_IPP
