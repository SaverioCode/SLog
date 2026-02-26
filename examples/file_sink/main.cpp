///
/// @file file_sink/main.cpp
/// @brief Demonstrates logging to a file using FileSink.
///
/// Shows:
///  - Creating a FileSink with a file path and write mode
///  - Writing log messages to a file
///  - Flushing the file sink
///

#include <slog/slog.hpp>
#include <slog/sinks/file_sink.hpp>

int main()
{
    std::shared_ptr<slog::Logger> logger = slog::Registry::instance().create_logger("file_logger");
    std::string_view file_name{"file_sink_output.log"};

    // Create a file sink — "w" mode truncates the file on each run
    auto file_sink = std::make_shared<slog::sinks::FileSink>("file_sink", file_name, "w");
    logger->add_sink(file_sink);

    logger->info("File logging started");
    logger->info("Writing integer: {}", 123);
    logger->info("Writing float: {:.2f}", 3.14159);
    logger->warn("This is a warning written to file");
    logger->error("This is an error written to file");

    // Ensure all output is flushed to disk
    logger->flush();

    // Using the default logger with the stdout console sink to notify
    SLOG_INFO("Done. Check {}", file_name);

    return 0;
}
