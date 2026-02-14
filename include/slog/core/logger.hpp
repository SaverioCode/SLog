#ifndef SLOG_CORE_LOGGER_HPP
#define SLOG_CORE_LOGGER_HPP

#include <chrono> // Todo: remove this when cleaned get_time_stamp
#include <format>
#include <memory>
#include <mutex>
#include <source_location>
#include <string>

#include <slog/config.hpp>
#include <slog/core/log_level.hpp>
#include <slog/core/log_proxy.hpp>
#include <slog/sinks/sink_manager.hpp>

// ------------------------
// Forward declarations
// ------------------------

namespace slog::core
{
    class Registry;
}

namespace slog::core
{

class Logger : public std::enable_shared_from_this<Logger>
{
public:
    ~Logger() = default;

#ifdef SLOG_REGISTRY_DISABLED
    Logger(std::string_view name);
    Logger(const std::shared_ptr<slog::sinks::ISink> sink);
    Logger(const std::vector<std::shared_ptr<slog::sinks::ISink>> sinks);
#endif

#ifdef SLOG_STREAM_ENABLED
    template<LogLevel level>
    SLOG_FORCE_INLINE auto log()
    {
        if constexpr (static_cast<uint8_t>(level) > SLOG_MAX_LOG_LEVEL) {
            return NullProxy{};
        }
        return LogProxy(shared_from_this(), level, level <= _local_log_level); 
    }
#endif

    template<LogLevel level, typename... Args>
    void log(std::format_string<Args...> fmt, Args&&... args)
    {
        std::string string_buffer;

        if constexpr (static_cast<uint8_t>(level) > SLOG_MAX_LOG_LEVEL) {
            return;
        }
        std::format_to(std::back_inserter(string_buffer), fmt, std::forward<Args>(args)...);
        _submit(level, std::move(string_buffer), std::source_location::current());
    }

#ifdef SLOG_STREAM_ENABLED
    SLOG_FORCE_INLINE auto fatal() { return log<LogLevel::FATAL>(); }
    SLOG_FORCE_INLINE auto error() { return log<LogLevel::ERROR>(); }
    SLOG_FORCE_INLINE auto warn()  { return log<LogLevel::WARNING>(); }
    SLOG_FORCE_INLINE auto info()  { return log<LogLevel::INFO>(); }
    SLOG_FORCE_INLINE auto debug() { return log<LogLevel::DEBUG>(); }
    SLOG_FORCE_INLINE auto trace() { return log<LogLevel::TRACE>(); }
#endif

    template<typename... Args>
    SLOG_FORCE_INLINE auto fatal(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::FATAL>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_FORCE_INLINE auto error(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::ERROR>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_FORCE_INLINE auto warn(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::WARNING>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_FORCE_INLINE auto info(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::INFO>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_FORCE_INLINE auto debug(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::DEBUG>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_FORCE_INLINE auto trace(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::TRACE>(fmt, std::forward<Args>(args)...);
    }

    void    addSink(std::shared_ptr<slog::sinks::ISink> sink);
    void    removeSink(const std::string& name) noexcept;

    [[nodiscard]] SLOG_FORCE_INLINE LogLevel    get_log_level() const noexcept
    {
        return _local_log_level;
    }

    SLOG_FORCE_INLINE void  set_log_level(const LogLevel level) noexcept
    {
        _local_log_level = level;
    }

    [[nodiscard]] SLOG_FORCE_INLINE std::string_view  get_name() const noexcept
    {
        return _name;
    }

private:
    friend struct LogProxy;
#ifndef SLOG_REGISTRY_DISABLED
    friend class  Registry;
#endif

    Logger() = delete;
#ifndef SLOG_REGISTRY_DISABLED
    Logger(std::string_view name);
    Logger(const std::shared_ptr<slog::sinks::ISink> sink);
    Logger(const std::vector<std::shared_ptr<slog::sinks::ISink>> sinks);
#endif
    Logger(Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger	operator=(Logger& other) = delete;
    Logger  operator=(Logger&& other) = delete;


    void    _submit(const LogLevel level, std::string&& message, std::source_location loc);

    // Todo: no need to split this functionn because it is going to be moved to the formatter or removed
    [[nodiscard]] std::string		_getTimeStamp()
    {
        char formatted[20];

        std::time_t timestamp = std::time(nullptr);
        std::tm* datetime = std::localtime(&timestamp);
        std::strftime(formatted, sizeof(formatted), "%Y-%m-%d %H:%M:%S", datetime);
        return std::string(formatted);
    }

    std::string               _name;
    slog::sinks::SinkManager  _sink_manager;
    LogLevel                  _local_log_level{LogLevel::TRACE}; // Todo: maybe rename it as logger log level? or something that clarify this
};

}

#endif // SLOG_CORE_LOGGER_HPP
