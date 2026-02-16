#include <array>
#include <cstdio>
#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <slog/slog.hpp>
#include <slog/sinks/console_sink.hpp>

TEST(ConsoleSinkTest, FlushAndWriteToTempFile)
{
    std::FILE* stream = std::tmpfile();
    ASSERT_NE(stream, nullptr) << "Failed to create temporary file for console sink test";

    const std::string message = "Testing console sink flush functionality";
    auto sink = std::make_shared<slog::sinks::ConsoleSink>("console_sink", stream);
    auto logger = slog::Registry::instance()->create_logger("console_test_logger");

    logger->add_sink(sink);

    logger->info("{}", message);
    logger->info() << message;
    logger->flush();

    // Go back to the beginning of the file
    std::rewind(stream);
    
    std::array<char, 256> buffer;
    std::size_t read_size = std::fread(buffer.data(), sizeof(char), buffer.size() - 1, stream);
    buffer[read_size] = '\0';

    // Verify the output contains our double message
    std::string output(buffer.data());
    std::string expected = message + message;
    EXPECT_EQ(output, expected);

    std::fclose(stream);
}
