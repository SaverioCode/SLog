///
/// @file custom_formatter/main.cpp
/// @brief Demonstrates custom format patterns with PatternFormatter.
///
/// Shows:
///  - Default pattern: "[%Y-%m-%d %H:%M:%S.%e] [%l] %v\n"
///  - Setting a custom pattern via logger.set_pattern()
///  - Available flags: %Y, %m, %d, %H, %M, %S, %e, %f, %l, %L, %t, %v,
///                     %s, %g, %#, %!, %n
///

#include <slog/slog.hpp>
#include <slog/sinks/console_sink.hpp>

int main()
{
    std::shared_ptr<slog::Logger> logger = slog::Registry::instance().get_default_logger_ptr();

    // ── Default pattern ──────────────────────────────────
    SLOG_INFO("Using the default pattern");

    // ── Compact pattern: level + message only ────────────
    logger->set_pattern("[%l] %v\n");
    SLOG_INFO("Compact: level and message only");
    SLOG_WARN("Compact warning");

    // ── Detailed pattern: time + thread + source + message
    logger->set_pattern("[%H:%M:%S.%e] [%l] [tid:%t] [%!] %v\n");
    SLOG_INFO("Detailed pattern with thread id and source function");

    // ── Logger name pattern ──────────────────────────────
    logger->set_pattern("[%n] [%L] %v\n");
    SLOG_INFO("Pattern showing logger name and short level");
    SLOG_ERROR("Error with logger name");

    // ── Minimal timestamp ────────────────────────────────
    logger->set_pattern("%H:%M:%S %v\n");
    SLOG_INFO("Minimal timestamp pattern");

    return 0;
}
