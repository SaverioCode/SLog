///
/// @file console_sink/main.cpp
/// @brief Demonstrates ConsoleSink with stdout and stderr.
///
/// Shows:
///  - Creating separate ConsoleSinks for stdout and stderr
///  - Using sink-level filtering to route errors to stderr
///  - Normal messages go to stdout, errors go to stderr
///

#include <slog/slog.hpp>
#include <slog/sinks/console_sink.hpp>

int main()
{
    slog::Logger& logger = slog::Registry::instance().get_default_logger();

    // stdout sink: it's automatically added by default

    // stderr sink: only ERROR and FATAL
    auto err = std::make_shared<slog::sinks::ConsoleSink>("stderr", stderr);
    err->set_level(slog::LogLevel::ERROR);
    logger.add_sink(err);

    SLOG_INFO("This goes to stdout only");
    SLOG_DEBUG("Debug: stdout only");
    SLOG_WARN("Warning: stdout only");
    SLOG_ERROR("Error: this goes to both stdout and stderr");
    SLOG_FATAL("Fatal: this also goes to both stdout and stderr");

    return 0;
}
