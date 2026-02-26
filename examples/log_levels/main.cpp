///
/// @file log_levels/main.cpp
/// @brief Demonstrates runtime log level filtering.
///
/// Shows:
///  - All levels: TRACE, DEBUG, INFO, WARNING, ERROR, FATAL
///  - Changing the logger level at runtime with set_log_level()
///  - Changing the global registry level with Registry::set_log_level()
///  - Only messages at or above the set level are emitted
///

#include <slog/slog.hpp>
#include <slog/sinks/console_sink.hpp>

int main()
{
    std::shared_ptr<slog::Logger> logger = slog::Registry::instance().get_default_logger_ptr();

    // All levels (default: TRACE)
    SLOG_INFO("Logger level: TRACE (all messages visible)");
    SLOG_TRACE("Trace message");
    SLOG_DEBUG("Debug message");
    SLOG_INFO("Info message");
    SLOG_WARN("Warning message");
    SLOG_ERROR("Error message");
    SLOG_FATAL("Fatal message");

    // Raise logger level to WARNING
    logger->set_log_level(slog::LogLevel::WARNING);
    SLOG_INFO("Logger level: WARNING");
    SLOG_TRACE("Trace: you should NOT see this");
    SLOG_DEBUG("Debug: you should NOT see this");
    SLOG_INFO("Info: you should NOT see this");
    SLOG_WARN("Warning: visible");
    SLOG_ERROR("Error: visible");

    // Raise logger level to ERROR
    logger->set_log_level(slog::LogLevel::ERROR);
    SLOG_INFO("Logger level: ERROR");
    SLOG_WARN("Warning: you should NOT see this");
    SLOG_ERROR("Error: visible");
    SLOG_FATAL("Fatal: visible");

    // Reset to TRACE
    logger->set_log_level(slog::LogLevel::TRACE);

    // Global registry level
    slog::Registry::instance().set_log_level(slog::LogLevel::ERROR);
    SLOG_DEBUG("Debug: filtered by registry level");
    SLOG_ERROR("Error: visible (passes registry level)");

    // Reset registry level
    slog::Registry::instance().set_log_level(slog::LogLevel::TRACE);
    SLOG_INFO("Registry level restored to TRACE");

    return 0;
}
