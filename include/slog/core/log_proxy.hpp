#ifndef SLOG_CORE_LOG_PROXY_HPP
#define SLOG_CORE_LOG_PROXY_HPP

namespace slog::core
{

class Logger;

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
    SLOG_FORCE_INLINE LogProxy(Logger& logger, LogLevel level,
        bool is_active = true, std::source_location loc = std::source_location::current()) :
        _logger(logger), _level(level), _is_active(is_active), _location(loc) {}

    LogProxy(const LogProxy&) = delete;
    LogProxy(LogProxy&&) = delete;

    ~LogProxy()
    {
        // Log level behaviour safeguard when logging is done calling methods instead of using MACROS
        if (!_is_active) [[unlikely]] {
            return;
        }
        // Format strings are preferred for performance
        if (_stream_buffer) [[unlikely]] {
            // The else is to prevent unintended usage from user
            // who might use both streams and format strings for one log line
            if (_string_buffer.empty()) [[likely]] {
                _string_buffer = std::move(*_stream_buffer).str();
            }
            else {
                _string_buffer.append(std::move(*_stream_buffer).str());
            }
        }
        _logger._submit(_level, std::move(_string_buffer), _location);
    }

    LogProxy& operator=(const LogProxy&) = delete;
    LogProxy& operator=(LogProxy&&) = delete;

    template<typename T>
    LogProxy& operator<<(const T& msg) { return _stream_write(msg); }
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

    SLOG_FORCE_INLINE void _ensure_stream()
    {
        if (!_stream_buffer) [[unlikely]] {
            _stream_buffer = std::make_unique<std::ostringstream>();
        }
    }
    template<typename V>
    LogProxy& _stream_write(V&& val)
    {
        _ensure_stream();
        (*_stream_buffer) << std::forward<V>(val);
        return *this;
    }

    Logger&                             _logger;
    LogLevel                            _level;
    bool                                _is_active;
    std::source_location                _location;
    std::string                         _string_buffer; 
    std::unique_ptr<std::ostringstream> _stream_buffer; 
};

struct VodifyLogProxy
{
    void operator&(const LogProxy&) {}
};

}

#endif // SLOG_CORE_LOG_PROXY_HPP
