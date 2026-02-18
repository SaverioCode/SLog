#ifndef SLOG_HPP
#define SLOG_HPP

#include <slog/common.hpp>
#include <slog/core/log_proxy.hpp>
#include <slog/core/logger.hpp>
#include <slog/core/registry.hpp>

#ifdef SLOG_HEADER_ONLY
    #include <slog/details/core/log_proxy.ipp>
    #include <slog/details/core/logger.ipp>
    #include <slog/details/core/registry.ipp>
#endif

#define SLOG_REGISTRY slog::Registry::instance()
#define SLOG_DEFAULT_LOGGER SLOG_REGISTRY->get_default_logger()
#define SLOG_GET_LOGGER(name) SLOG_REGISTRY->get_logger(name)

// ---------------------------------
// Logging macros for Default Logger
// ---------------------------------

#define SLOG_FATAL SLOG(slog::LogLevel::FATAL, SLOG_DEFAULT_LOGGER)
#define SLOG_ERROR SLOG(slog::LogLevel::ERROR, SLOG_DEFAULT_LOGGER)
#define SLOG_WARN SLOG(slog::LogLevel::WARNING, SLOG_DEFAULT_LOGGER)
#define SLOG_INFO SLOG(slog::LogLevel::INFO, SLOG_DEFAULT_LOGGER)
#define SLOG_DEBUG SLOG(slog::LogLevel::DEBUG, SLOG_DEFAULT_LOGGER)
#define SLOG_TRACE SLOG(slog::LogLevel::TRACE, SLOG_DEFAULT_LOGGER)

// ---------------------------------
// Logging macros by Logger name
// ---------------------------------

#define SLOG_FATAL_N(name) SLOG(slog::LogLevel::FATAL, SLOG_GET_LOGGER(name))
#define SLOG_ERROR_N(name) SLOG(slog::LogLevel::ERROR, SLOG_GET_LOGGER(name))
#define SLOG_WARN_N(name) SLOG(slog::LogLevel::WARNING, SLOG_GET_LOGGER(name))
#define SLOG_INFO_N(name) SLOG(slog::LogLevel::INFO, SLOG_GET_LOGGER(name))
#define SLOG_DEBUG_N(name) SLOG(slog::LogLevel::DEBUG, SLOG_GET_LOGGER(name))
#define SLOG_TRACE_N(name) SLOG(slog::LogLevel::TRACE, SLOG_GET_LOGGER(name))

// ---------------------------------
// Logging macros by Logger instance
// ---------------------------------

#define SLOG_FATAL_L(logger) SLOG(slog::LogLevel::FATAL, logger)
#define SLOG_ERROR_L(logger) SLOG(slog::LogLevel::ERROR, logger)
#define SLOG_WARN_L(logger) SLOG(slog::LogLevel::WARNING, logger)
#define SLOG_INFO_L(logger) SLOG(slog::LogLevel::INFO, logger)
#define SLOG_DEBUG_L(logger) SLOG(slog::LogLevel::DEBUG, logger)
#define SLOG_TRACE_L(logger) SLOG(slog::LogLevel::TRACE, logger)

// ---------------------------------
// Base Logging Macro
// ---------------------------------

#ifdef SLOG_STREAM_ENABLED
    #define SLOG(lvl, logger)                                                                      \
        ((static_cast<uint8_t>(lvl) > SLOG_MAX_LOG_LEVEL) || _SLOG_IS_OFF(lvl, logger))            \
            ? (void)0                                                                              \
            : slog::VodifyLogProxy() & slog::LogProxy(logger, lvl)
#else
    #define SLOG(lvl, logger)                                                                      \
        ((static_cast<uint8_t>(lvl) > SLOG_MAX_LOG_LEVEL) || _SLOG_IS_OFF(lvl, logger))            \
            ? (void)0                                                                              \
            : logger->log<lvl>
#endif

#define _SLOG_IS_OFF(lvl, logger)                                                                  \
    (lvl > SLOG_REGISTRY->get_log_level()) || (lvl > logger->get_log_level())

#endif // SLOG_HPP
