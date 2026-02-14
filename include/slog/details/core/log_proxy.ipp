
#ifdef SLOG_STREAM_ENABLED
#include <iostream>
#endif

#include <slog/core/logger.hpp>
#include <slog/core/log_level.hpp>

namespace slog::core
{

// --------------
// PUBLIC
// --------------

SLOG_FORCE_INLINE LogProxy::LogProxy(std::shared_ptr<Logger> logger, LogLevel level, bool is_active, std::source_location loc) :
    _logger(logger), _level(level), _is_active(is_active), _location(loc) {}


#ifdef SLOG_STREAM_ENABLED
inline LogProxy::~LogProxy()
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
    _submit(_level, std::move(_string_buffer), _location);
}
#else
SLOG_FORCE_INLINE LogProxy::~LogProxy()
{
    _submit(_level, std::move(_string_buffer), _location);
}
#endif

// --------------
// PRIVATE
// --------------

#ifdef SLOG_STREAM_ENABLED
SLOG_FORCE_INLINE void LogProxy::_ensure_stream()
{
    if (!_stream_buffer) [[unlikely]] {
        _stream_buffer = std::make_unique<std::ostringstream>();
    }
}
#endif

SLOG_FORCE_INLINE void LogProxy::_submit(LogLevel level, std::string&& buffer, std::source_location location)
{
    if (_logger) [[likely]] {
        _logger->_submit(level, std::move(buffer), location);
    }
}

}
