#include <slog/slog.hpp>

int main(void)
{
    slog::Logger& logger = slog::Logger::getInstance();

    std::shared_ptr<slog::ISink> console_sink = std::make_shared<slog::StreamSink>("console", std::cout);
    logger.addSink(console_sink);

    SLOG_INFO("Simple-Logger C++17 example started.");
    SLOG_INFO("Logging an integer: {}", 42);
    SLOG_INFO("Logging with complex multi type format: {}, {}, {}", "string", 3.1415, 2024);
    SLOG_INFO << "Logging using stream operator: " << 3.14;
    SLOG_INFO << "Logging using stream operator: " << std::hex << 255 << std::endl;


    logger.info() << "Logging info level message via Logger instance.";
    logger.info() << "Logging info level message via Logger instance with stream operator: " << std::hex << 512 << std::endl;
    logger.info("Logging info level message with format: {}", "Hello, World!");
    
    // Note: Non intended syntax that is also supported as collateral effect 
    // to unify MACRO interface for both stream and format style logging
    SLOG_INFO("Logging an integer: {}", 42) << " Crazy shit!";
    logger.info()("Logging info level message with format: {}", "Hello, World!") << " Crazy shit!";

}