#ifndef SLOG_HPP
#define SLOG_HPP

#define SLOG_FATAL SLOG(slog::LogLevel::FATAL)
#define SLOG_ERROR SLOG(slog::LogLevel::ERROR)
#define SLOG_WARN SLOG(slog::LogLevel::WARNING)
#define SLOG_INFO SLOG(slog::LogLevel::INFO)
#define SLOG_DEBUG SLOG(slog::LogLevel::DEBUG)
#define SLOG_TRACE SLOG(slog::LogLevel::TRACE)

#define SLOG(lvl) \
    ((static_cast<uint8_t>(lvl) > SLOG_MAX_LOG_LEVEL) && SLOG_IS_ON(lvl)) ? \
        (void)0 : slog::VodifyLogProxy() & slog::LogProxy(slog::Logger::getInstance(), lvl)

#define SLOG_IS_ON(lvl) \
    (slog::Logger::getInstance().getGlobalLevel() < lvl)

namespace slog
{

}

#endif // SLOGGER_HPP
