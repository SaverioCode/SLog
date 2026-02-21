#ifndef SLOG_CORE_LOG_PROXY_HPP
#define SLOG_CORE_LOG_PROXY_HPP

#ifndef SLOG_STREAM_DISABLED

#include <format>
#include <iosfwd>
#include <memory>
#include <source_location>
#include <utility>

#include <slog/common.hpp>
#include <slog/core/log_level.hpp>

// ------------------------
// Forward declarations
// ------------------------

namespace slog
{
class Logger;
}

namespace slog
{

struct NullProxy
{
    template<typename T>
    constexpr NullProxy& operator<<(const T&) noexcept
    {
        return *this;
    }
    NullProxy& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        (void)manip;
        return *this;
    }
    NullProxy& operator<<(std::ios_base& (*manip)(std::ios_base&))
    {
        (void)manip;
        return *this;
    }

    template<typename... Args>
    constexpr void operator()(std::string_view, Args&&...) noexcept
    {
    }
};

class LogProxy
{
public:
    LogProxy(std::shared_ptr<Logger> logger, LogLevel level, bool is_active = true,
             std::source_location loc = std::source_location::current());
    LogProxy(const LogProxy&) = delete;
    LogProxy(LogProxy&&) = delete;

    ~LogProxy();

    LogProxy& operator=(const LogProxy&) = delete;
    LogProxy& operator=(LogProxy&&) = delete;

    template<typename T>
    LogProxy& operator<<(const T& msg)
    {
        return _stream_write(msg);
    }
    LogProxy& operator<<(std::ostream& (*manip)(std::ostream&)) { return _stream_write(manip); }
    LogProxy& operator<<(std::ios_base& (*manip)(std::ios_base&)) { return _stream_write(manip); }

    template<typename... Args>
    LogProxy& operator()(std::format_string<Args...> fmt, Args&&... args)
    {
        std::format_to(std::back_inserter(_string_buffer), fmt, std::forward<Args>(args)...);
        return *this;
    }

private:
    friend class Logger;

    void _ensure_stream();

    template<typename V>
    LogProxy& _stream_write(V&& val)
    {
        _ensure_stream();
        (*_stream_buffer) << std::forward<V>(val);
        return *this;
    }

    void _submit(LogLevel level, std::string&& message, std::source_location location);

    std::shared_ptr<Logger> _logger;
    LogLevel _level;
    bool _is_active;
    std::source_location _location;
    std::string _string_buffer;
    std::unique_ptr<std::ostringstream> _stream_buffer;
};

struct VodifyLogProxy
{
    void operator&(const LogProxy&) {}
};

} // namespace slog

#endif // SLOG_STREAM_DISABLED

#endif // SLOG_CORE_LOG_PROXY_HPP
