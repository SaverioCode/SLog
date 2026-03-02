#include <string>

#include <slog/fmt/pattern_formatter.hpp>
#include <slog/slog.hpp>

#if defined(_WIN32)
    #define PLUGIN_API __declspec(dllexport)
#else
    #define PLUGIN_API __attribute__((visibility("default")))
#endif

extern "C" PLUGIN_API bool verify_registry_instance_test(slog::LogLevel expected_level)
{
    // The main executable will change the default log level
    // and will check if it will be consistent in the shared library too
    return SLOG_REGISTRY.get_log_level() == expected_level;
}

extern "C" PLUGIN_API bool verify_formatter_instance_test()
{
    // The main test executable registered a custom flag 'X'.
    // We create a dummy formatter, format with '%X' and see if it outputs "[SHARED]".
    slog::fmt::PatternFormatter formatter("%X");
    
    // Create a dummy log record using aggregate initialization
    slog::LogRecord dummy{};
    dummy.logger_name = "dummy_logger";
    dummy.level = slog::LogLevel::INFO;
    dummy.message = "dummy message";
    
    std::string result;
    formatter.format(dummy, result);
    
    // If the _dispatch_table singleton was NOT shared, this would output the literal "%X"
    return result == "[SHARED]";
}
