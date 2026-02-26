///
/// @file multi_sink/main.cpp
/// @brief Demonstrates attaching multiple sinks to a single logger.
///
/// Shows:
///  - ConsoleSink writing to stdout
///  - FileSink writing to a log file
///  - One log call producing output in both destinations
///

#include <slog/slog.hpp>
#include <slog/sinks/console_sink.hpp>
#include <slog/sinks/file_sink.hpp>

int main()
{
    std::shared_ptr<slog::Logger> logger = slog::Registry::instance().create_logger("multi_sink");

    // Necessary to use the SLOG_INFO macro instead of SLOG_INFO_L
    slog::Registry::instance().set_default_logger_name("multi_sink");

    // Attach a console sink (stdout)
    auto console = std::make_shared<slog::sinks::ConsoleSink>("stdout", stdout);
    logger->add_sink(console);

    // Attach a file sink
    auto file = std::make_shared<slog::sinks::FileSink>("file_sink", "multi_sink_output.log");
    logger->add_sink(file);

    // Both sinks receive every log message
    SLOG_INFO("This message goes to both console and file");
    SLOG_WARN("Warning: check multi_sink_output.log for the file copy");
    SLOG_INFO("Formatted: answer={}", 42);
    logger->error("Error: this message goes to both console and file");

    // Flush to ensure file output is written
    logger->flush();

    SLOG_INFO("Done. Check multi_sink_output.log for file output.");

    return 0;
}
