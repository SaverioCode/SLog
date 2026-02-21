#ifndef SLOG_CORE_LOGGER_HPP
#define SLOG_CORE_LOGGER_HPP

#include <iosfwd>
#include <source_location>

#include <slog/async/worker.hpp>
#include <slog/common.hpp>
#include <slog/core/log_level.hpp>
#include <slog/core/log_proxy.hpp>
#include <slog/sinks/sink_manager.hpp>

// ------------------------
// Forward declarations
// ------------------------

namespace slog
{
class Registry;
}

namespace slog
{

class Logger : public std::enable_shared_from_this<Logger>
{
public:
    ~Logger() = default;

#ifdef SLOG_STREAM_ENABLED
    template<LogLevel level>
    SLOG_ALWAYS_INLINE auto log()
    {
        if constexpr (static_cast<uint8_t>(level) > SLOG_MAX_LOG_LEVEL) {
            return NullProxy{};
        }
        else {
            return LogProxy(shared_from_this(), level, level <= _log_level);
        }
    }
#endif

    template<LogLevel level, typename... Args>
    void log(std::format_string<Args...> fmt, Args&&... args)
    {
        std::string string_buffer;

        if constexpr (static_cast<uint8_t>(level) > SLOG_MAX_LOG_LEVEL) {
            return;
        }
        if (level > _log_level) {
            return;
        }
        std::format_to(std::back_inserter(string_buffer), fmt, std::forward<Args>(args)...);
        _submit(level, std::move(string_buffer), std::source_location::current());
    }

#ifdef SLOG_STREAM_ENABLED
    SLOG_ALWAYS_INLINE auto fatal() { return log<LogLevel::FATAL>(); }
    SLOG_ALWAYS_INLINE auto error() { return log<LogLevel::ERROR>(); }
    SLOG_ALWAYS_INLINE auto warn() { return log<LogLevel::WARNING>(); }
    SLOG_ALWAYS_INLINE auto info() { return log<LogLevel::INFO>(); }
    SLOG_ALWAYS_INLINE auto debug() { return log<LogLevel::DEBUG>(); }
    SLOG_ALWAYS_INLINE auto trace() { return log<LogLevel::TRACE>(); }
#endif

    template<typename... Args>
    SLOG_ALWAYS_INLINE void fatal(std::format_string<Args...> fmt, Args&&... args)
    {
        log<LogLevel::FATAL>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_ALWAYS_INLINE void error(std::format_string<Args...> fmt, Args&&... args)
    {
        log<LogLevel::ERROR>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_ALWAYS_INLINE void warn(std::format_string<Args...> fmt, Args&&... args)
    {
        log<LogLevel::WARNING>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_ALWAYS_INLINE void info(std::format_string<Args...> fmt, Args&&... args)
    {
        log<LogLevel::INFO>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_ALWAYS_INLINE void debug(std::format_string<Args...> fmt, Args&&... args)
    {
        log<LogLevel::DEBUG>(fmt, std::forward<Args>(args)...);
    }
    template<typename... Args>
    SLOG_ALWAYS_INLINE void trace(std::format_string<Args...> fmt, Args&&... args)
    {
        log<LogLevel::TRACE>(fmt, std::forward<Args>(args)...);
    }

    void flush() const;

    void add_sink(std::shared_ptr<slog::sinks::ISink> sink);
    void remove_sink(const std::string& name) noexcept;

    [[nodiscard]] std::shared_ptr<slog::sinks::ISink> get_sink(const std::string& name) const;
    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<slog::sinks::ISink>>> get_sinks() const;

    [[nodiscard]] SLOG_ALWAYS_INLINE LogLevel get_log_level() const noexcept { return _log_level; }

    SLOG_ALWAYS_INLINE void set_log_level(const LogLevel level) noexcept { _log_level = level; }

    [[nodiscard]] SLOG_ALWAYS_INLINE std::string_view get_name() const noexcept { return _name; }

private:
#ifndef SLOG_STREAM_DISABLED
    friend class LogProxy;
#endif
    friend class Registry;

    Logger() = delete;
    Logger(std::string_view name, std::shared_ptr<slog::async::Worker> worker);
    Logger(std::string_view name, const std::shared_ptr<slog::sinks::ISink> sink, std::shared_ptr<slog::async::Worker> worker);
    Logger(std::string_view name, const std::vector<std::shared_ptr<slog::sinks::ISink>> sinks, std::shared_ptr<slog::async::Worker> worker);
    Logger(Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger operator=(Logger& other) = delete;
    Logger operator=(Logger&& other) = delete;

    void _submit(const LogLevel level, std::string&& message, std::source_location loc);

    // Todo: no need to split this functionn because it is going to be moved to the formatter or
    // removed
    [[nodiscard]] std::string _getTimeStamp();

    std::string _name;
    LogLevel _log_level{LogLevel::TRACE};
    std::shared_ptr<slog::sinks::SinkManager> _sink_manager{nullptr};
    std::shared_ptr<slog::async::Worker> _worker;
};

} // namespace slog

#endif // SLOG_CORE_LOGGER_HPP
