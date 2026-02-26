///
/// @file basic_logging/main.cpp
/// @brief Demonstrates basic logging with SLOG macros, format-style and stream-style.
///
/// Shows:
///  - Using SLOG_INFO, SLOG_DEBUG, SLOG_WARN, SLOG_ERROR with std::format args
///  - Using SLOG_INFO << ... stream-style syntax
///  - Using logger instance methods (logger.info(), logger.info() << ...)
///

#include <slog/slog.hpp>
#include <slog/sinks/console_sink.hpp>

int main()
{
    // ── Format-style logging (macros) ────────────────────
    SLOG_INFO("Hello from SLog!");
    SLOG_INFO("Logging an integer: {}", 42);
    SLOG_INFO("Multiple args: name={}, pi={:.4f}, year={}", "SLog", 3.14159, 2026);
    SLOG_DEBUG("This is a debug message");
    SLOG_WARN("This is a warning");
    SLOG_ERROR("Something went wrong: error code {}", -1);

    // ── Stream-style logging (macros) ────────────────────
    SLOG_INFO << "Stream-style logging: " << 3.14;
    SLOG_INFO << "Hex value: " << std::hex << 255;
    SLOG_WARN << "Warning via stream";
    SLOG_DEBUG << "Debug value: " << 42;

    // ── Instance-based logging ───────────────────────────
    std::shared_ptr<slog::Logger> logger = slog::Registry::instance().get_default_logger_ptr();
    logger->info("Logger instance: format style with arg={}", 100);
    logger->info() << "Logger instance: stream style";

    return 0;
}
