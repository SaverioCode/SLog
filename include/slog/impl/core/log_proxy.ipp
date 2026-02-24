
#ifndef SLOG_CORE_LOG_PROXY_IPP
#define SLOG_CORE_LOG_PROXY_IPP

#ifndef SLOG_STREAM_DISABLED

    #include <iostream>

    #include <slog/core/log_level.hpp>
    #include <slog/core/logger.hpp>
    #include <slog/details/thread_id.hpp>

namespace slog
{

// --------------
// PUBLIC
// --------------

SLOG_ALWAYS_INLINE LogProxy::LogProxy(std::shared_ptr<Logger> logger, LogLevel level,
                                      bool is_active, std::source_location loc)
    : _logger(logger), _is_active(is_active)
{
    _record.level = level;
    _record.location = loc;
    _record.timestamp = std::chrono::system_clock::now();
    _record.thread_id = slog::details::current_thread_id();
}

SLOG_INLINE LogProxy::~LogProxy()
{
    // Log level behaviour safeguard when logging is done calling methods instead of using MACROS
    if (!_is_active) [[unlikely]] {
        return;
    }
    // Format strings are preferred for performance
    if (_stream_buffer) [[unlikely]] {
        _record.string_buffer = std::move(*_stream_buffer).str();
    }
    if (_logger) [[likely]] {
        _logger->_submit(std::move(_record));
    }
}

// --------------
// PRIVATE
// --------------

SLOG_ALWAYS_INLINE void LogProxy::_ensure_stream()
{
    if (!_stream_buffer) [[unlikely]] {
        _stream_buffer = std::make_unique<std::ostringstream>();
    }
}

} // namespace slog

#endif // SLOG_STREAM_DISABLED

#endif // SLOG_CORE_LOG_PROXY_IPP
