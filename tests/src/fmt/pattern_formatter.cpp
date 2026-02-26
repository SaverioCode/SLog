#include <gtest/gtest.h>
#include <chrono>
#include <format>
#include <string>

#include <slog/core/log_record.hpp>
#include <slog/details/time.hpp>
#include <slog/fmt/pattern_formatter.hpp>

// ============================================================================
// Shared fixture — one LogRecord reused across all PatternFormatter tests
// ============================================================================

class PatternFormatterTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        auto day = std::chrono::sys_days{std::chrono::year{2026}/std::chrono::month{2}/std::chrono::day{23}};
        auto tp  = day + std::chrono::hours{14} + std::chrono::minutes{30} + std::chrono::seconds{45}
                       + std::chrono::milliseconds{123} + std::chrono::microseconds{456};

        record.level         = slog::LogLevel::INFO;
        record.logger_name   = "app";
        record.string_buffer = "test message";
        record.location      = std::source_location::current();
        record.timestamp     = tp;
        record.thread_id     = 99;
        record.format_fn     = nullptr;
    }

    slog::LogRecord record;
    std::string dest;
};

// ---- Core formatting tests ----

TEST_F(PatternFormatterTest, LevelAndMessage)
{
    slog::fmt::PatternFormatter fmt("[%l] %v");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "[INFO] test message");
}

TEST_F(PatternFormatterTest, LiteralPercent)
{
    slog::fmt::PatternFormatter fmt("100%% done");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "100% done");
}

TEST_F(PatternFormatterTest, UnknownFlagAsLiteral)
{
    slog::fmt::PatternFormatter fmt("%Z");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "%Z");
}

TEST_F(PatternFormatterTest, EmptyPattern)
{
    slog::fmt::PatternFormatter fmt("");
    fmt.format(record, dest);
    EXPECT_TRUE(dest.empty());
}

TEST_F(PatternFormatterTest, OnlyLiterals)
{
    slog::fmt::PatternFormatter fmt("hello world");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "hello world");
}

TEST_F(PatternFormatterTest, OnlyFlags)
{
    slog::fmt::PatternFormatter fmt("%l%L%t");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "INFOI99");
}

TEST_F(PatternFormatterTest, SetPatternChangesOutput)
{
    slog::fmt::PatternFormatter fmt("%l");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "INFO");

    dest.clear();
    fmt.set_pattern("%v");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "test message");
}

TEST_F(PatternFormatterTest, GetPatternReturnsPattern)
{
    slog::fmt::PatternFormatter fmt("%l %v");
    EXPECT_EQ(fmt.get_pattern(), "%l %v");
}

TEST_F(PatternFormatterTest, ThreadIdAndLoggerName)
{
    slog::fmt::PatternFormatter fmt("[%n] [%t] %v");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "[app] [99] test message");
}

TEST_F(PatternFormatterTest, MultiLevelPattern)
{
    slog::fmt::PatternFormatter fmt("%L/%s:%# %v");
    fmt.format(record, dest);

    // Starts with short level
    EXPECT_EQ(dest[0], 'I');
    EXPECT_EQ(dest[1], '/');
    // Contains the message at the end
    EXPECT_NE(dest.find("test message"), std::string::npos);
}

// ---- Time formatting test (timezone-aware) ----

TEST_F(PatternFormatterTest, DateTimeMillis)
{
    slog::fmt::PatternFormatter fmt("%Y-%m-%d %H:%M:%S.%e");
    fmt.format(record, dest);

    // Derive expected local time
    std::time_t tt = std::chrono::system_clock::to_time_t(record.timestamp);
    std::tm local{};
    slog::details::localtime(&tt, &local);

    std::string expected = std::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.123",
        local.tm_year + 1900, local.tm_mon + 1, local.tm_mday,
        local.tm_hour, local.tm_min, local.tm_sec);

    EXPECT_EQ(dest, expected);
}

TEST_F(PatternFormatterTest, TrailingPercentSign)
{
    // A pattern ending with a lone '%' — should be treated as literal
    slog::fmt::PatternFormatter fmt("hello%");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "hello%");
}

TEST_F(PatternFormatterTest, RegisterCustomFlag)
{
    slog::fmt::PatternFormatter::register_flag('X', [](const slog::fmt::FormatContext&, std::string& d) {
        d.append("CUSTOM");
    });

    slog::fmt::PatternFormatter fmt("%X");
    fmt.format(record, dest);
    EXPECT_EQ(dest, "CUSTOM");

    // Cleanup: unregister by setting nullptr won't work since register_flag
    // only sets non-null. But since the table is shared, we note that this
    // flag will persist for the process lifetime.
}
