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
#include <slog/core/log_record.hpp>
#include <slog/fmt/deferred_format.hpp>

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
    void operator()(std::format_string<Args...> fmt, Args&&... args)
    {
        _record.format_str = fmt.get();
        _record.format_fn = &slog::fmt::format_deferred<std::decay_t<Args>...>;
        _record.stored_args.emplace<std::tuple<std::decay_t<Args>...>>(std::forward<Args>(args)...);
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

    LogRecord _record;
    std::shared_ptr<Logger> _logger;
    bool _is_active;
    std::unique_ptr<std::ostringstream> _stream_buffer;
};

struct VodifyLogProxy
{
    void operator&(const LogProxy&) {}
};

} // namespace slog

#endif // SLOG_STREAM_DISABLED

#endif // SLOG_CORE_LOG_PROXY_HPP
