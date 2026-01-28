#ifndef SLOG_HPP
#define SLOG_HPP

#include <slog/config.hpp>
#include <slog/core/logger.hpp>
#include <slog/core/log_proxy.hpp>

#ifdef SLOG_HEADER_ONLY
    #include <slog/details/core/logger.ipp>  
    #include <slog/details/core/log_proxy.ipp>
#endif

#define SLOG_FATAL SLOG(slog::core::LogLevel::FATAL)
#define SLOG_ERROR SLOG(slog::core::LogLevel::ERROR)
#define SLOG_WARN SLOG(slog::core::LogLevel::WARNING)
#define SLOG_INFO SLOG(slog::core::LogLevel::INFO)
#define SLOG_DEBUG SLOG(slog::core::LogLevel::DEBUG)
#define SLOG_TRACE SLOG(slog::core::LogLevel::TRACE)

#define SLOG(lvl) \
    ((static_cast<uint8_t>(lvl) > SLOG_MAX_LOG_LEVEL) && SLOG_IS_ON(lvl)) ? \
        (void)0 : slog::core::VodifyLogProxy() & slog::core::LogProxy(slog::core::Logger::getInstance(), lvl)

#define SLOG_IS_ON(lvl) \
    (slog::core::Logger::getInstance().getGlobalLevel() < lvl)

#endif // SLOG_HPP
