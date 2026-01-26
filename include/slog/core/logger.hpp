#ifndef SLOG_CORE_LOGGER_HPP
#define SLOG_CORE_LOGGER_HPP

#include <chrono>
#include <format>
#include <memory>
#include <sstream>
#include <string>

#include <slog/config.hpp>
#include <slog/core/log_record.hpp>

namespace slog::core
{

class Logger
{
public:
    ~Logger() = default;

    SLOG_FORCE_INLINE static Logger&	getInstance(const int options = 0)
    {
        struct Temp : public Logger { Temp(const int tmp_options) : Logger(tmp_options) {} };
        std::call_once(_once_flag, [&]() { _instance = std::make_unique<Temp>(options); });
        return *_instance;
    }

    template<LogLevel level>
    SLOG_FORCE_INLINE auto log()
    {
        if constexpr (static_cast<uint8_t>(level) > SLOG_MAX_LOG_LEVEL) {
            return NullProxy{};
        }
        return LogProxy(*this, level, level <= _global_log_level); 
    }

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

    SLOG_FORCE_INLINE auto fatal() { return log<LogLevel::FATAL>(); }
    SLOG_FORCE_INLINE auto error() { return log<LogLevel::ERROR>(); }
    SLOG_FORCE_INLINE auto warn()  { return log<LogLevel::WARNING>(); }
    SLOG_FORCE_INLINE auto info()  { return log<LogLevel::INFO>(); }
    SLOG_FORCE_INLINE auto debug() { return log<LogLevel::DEBUG>(); }
    SLOG_FORCE_INLINE auto trace() { return log<LogLevel::TRACE>(); }

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

    SLOG_FORCE_INLINE void    addSink(std::shared_ptr<ISink> sink)
    {
        _sink_manager.addSink(sink);
    }

    SLOG_FORCE_INLINE void    removeSink(const std::string& name) noexcept
    {
        _sink_manager.removeSink(name);
    }

    [[nodiscard]] SLOG_FORCE_INLINE LogLevel    getGlobalLevel() const noexcept
    {
        return _global_log_level;
    }

    SLOG_FORCE_INLINE void  setGlobalLevel(const LogLevel level) noexcept
    {
        _global_log_level = level;
    }

private:
    friend struct LogProxy;

    Logger() = delete;
    Logger(const int options) {} // Todo: option for having an out of the box std::cout sink?
    Logger(Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger	operator=(Logger& other) = delete;
    Logger  operator=(Logger&& other) = delete;


    void            _submit(const LogLevel level, std::string&& message, std::source_location loc)
    {
        LogRecord record{level, std::move(message), loc};

        _sink_manager.dispatch(record);
    }

    [[nodiscard]] std::string		_getTimeStamp()
    {
        char formatted[20];

        std::time_t timestamp = std::time(nullptr);
        std::tm* datetime = std::localtime(&timestamp);
        std::strftime(formatted, sizeof(formatted), "%Y-%m-%d %H:%M:%S", datetime);
        return std::string(formatted);
    }

    inline static std::unique_ptr<Logger>   _instance{nullptr};
    inline static SinkManager               _sink_manager{};
    inline static std::once_flag    	    _once_flag{};
    inline static std::mutex	            _log_mutex{};
    LogLevel                                _global_log_level{LogLevel::TRACE};
};

}

#endif // SLOG_CORE_LOGGER_HPP
