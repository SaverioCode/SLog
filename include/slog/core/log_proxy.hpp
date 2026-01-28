#ifndef SLOG_CORE_LOG_PROXY_HPP
#define SLOG_CORE_LOG_PROXY_HPP

#ifdef SLOG_STREAM_ENABLED
    #include <iosfwd>
    #include <memory>
#endif

#include <string>
#include <format>
#include <utility>

#include <slog/config.hpp>
#include <slog/core/log_level.hpp>

// ------------------------
// Forward declarations
// ------------------------

namespace slog::core
{
    class Logger;
}

namespace slog::core
{

struct NullProxy
{
    template<typename T>
    constexpr NullProxy& operator<<(const T&) noexcept { return *this; }

    template<typename... Args>
    constexpr void operator()(std::string_view, Args&&...) noexcept {}
};

class LogProxy
{
public:
    LogProxy(Logger& logger, LogLevel level,
        bool is_active = true, std::source_location loc = std::source_location::current());
    LogProxy(const LogProxy&) = delete;
    LogProxy(LogProxy&&) = delete;

    ~LogProxy();

    LogProxy& operator=(const LogProxy&) = delete;
    LogProxy& operator=(LogProxy&&) = delete;

#ifdef SLOG_STREAM_ENABLED
    template<typename T>
    LogProxy& operator<<(const T& msg) { return _stream_write(msg); }
    LogProxy& operator<<(std::ostream& (*manip)(std::ostream&)) { return _stream_write(manip); }
    LogProxy& operator<<(std::ios_base& (*manip)(std::ios_base&)) { return _stream_write(manip); }
#endif

    template<typename... Args>
    LogProxy& operator()(std::format_string<Args...> fmt, Args&&... args)
    {
        std::format_to(std::back_inserter(_string_buffer), fmt, std::forward<Args>(args)...);
        return *this;
    }

private:
    friend class Logger;

#ifdef SLOG_STREAM_ENABLED
    void _ensure_stream();

    template<typename V>
    LogProxy& _stream_write(V&& val)
    {
        _ensure_stream();
        (*_stream_buffer) << std::forward<V>(val);
        return *this;
    }
#endif

    void _submit(LogLevel level, std::string&& message, std::source_location location);

    Logger&                             _logger;
    LogLevel                            _level;
    bool                                _is_active;
    std::source_location                _location;
    std::string                         _string_buffer; 
#ifdef SLOG_STREAM_ENABLED
    std::unique_ptr<std::ostringstream> _stream_buffer; 
#endif
};

struct VodifyLogProxy
{
    void operator&(const LogProxy&) {}
};

}

#endif // SLOG_CORE_LOG_PROXY_HPP
