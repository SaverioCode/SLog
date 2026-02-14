#ifndef SLOG_HPP
#define SLOG_HPP

#include <slog/config.hpp>
#include <slog/core/logger.hpp>
#include <slog/core/log_proxy.hpp>
#include <slog/core/registry.hpp>

#ifdef SLOG_HEADER_ONLY
    #include <slog/details/core/logger.ipp>  
    #include <slog/details/core/log_proxy.ipp>
    #include <slog/details/core/registry.ipp>
#endif

#define SLOG_REGISTRY slog::core::Registry::instance()
#define SLOG_DEFAULT_LOGGER SLOG_REGISTRY->get_default_logger()
#define SLOG_GET_LOGGER(name) SLOG_REGISTRY->get_logger(name)

#define SLOG_FATAL SLOG(slog::core::LogLevel::FATAL, SLOG_DEFAULT_LOGGER)
#define SLOG_ERROR SLOG(slog::core::LogLevel::ERROR, SLOG_DEFAULT_LOGGER)
#define SLOG_WARN SLOG(slog::core::LogLevel::WARNING, SLOG_DEFAULT_LOGGER)
#define SLOG_INFO SLOG(slog::core::LogLevel::INFO, SLOG_DEFAULT_LOGGER)
#define SLOG_DEBUG SLOG(slog::core::LogLevel::DEBUG, SLOG_DEFAULT_LOGGER)
#define SLOG_TRACE SLOG(slog::core::LogLevel::TRACE, SLOG_DEFAULT_LOGGER)

#define SLOG_FATAL_N(name) SLOG(slog::core::LogLevel::FATAL, SLOG_GET_LOGGER(name))
#define SLOG_ERROR_N(name) SLOG(slog::core::LogLevel::ERROR, SLOG_GET_LOGGER(name))
#define SLOG_WARN_N(name) SLOG(slog::core::LogLevel::WARNING, SLOG_GET_LOGGER(name))
#define SLOG_INFO_N(name) SLOG(slog::core::LogLevel::INFO, SLOG_GET_LOGGER(name))
#define SLOG_DEBUG_N(name) SLOG(slog::core::LogLevel::DEBUG, SLOG_GET_LOGGER(name))
#define SLOG_TRACE_N(name) SLOG(slog::core::LogLevel::TRACE, SLOG_GET_LOGGER(name))

#define SLOG_FATAL_L(logger) SLOG(slog::core::LogLevel::FATAL, logger)
#define SLOG_ERROR_L(logger) SLOG(slog::core::LogLevel::ERROR, logger)
#define SLOG_WARN_L(logger) SLOG(slog::core::LogLevel::WARNING, logger)
#define SLOG_INFO_L(logger) SLOG(slog::core::LogLevel::INFO, logger)
#define SLOG_DEBUG_L(logger) SLOG(slog::core::LogLevel::DEBUG, logger)
#define SLOG_TRACE_L(logger) SLOG(slog::core::LogLevel::TRACE, logger)


#define SLOG(lvl, logger) \
    ((static_cast<uint8_t>(lvl) > SLOG_MAX_LOG_LEVEL) && _SLOG_IS_ON(lvl, logger)) ? \
        (void)0 : slog::core::VodifyLogProxy() & slog::core::LogProxy(logger, lvl)

#ifndef SLOG_REGISTRY_DISABLED
    #define _SLOG_IS_ON(lvl, logger) \
        (logger->get_log_level() <= lvl)
#else
    #define _SLOG_IS_ON(lvl, logger) \
        (SLOG_REGISTRY->get_log_level() <= lvl) || (logger->get_log_level() <= lvl)
#endif

#endif // SLOG_HPP
