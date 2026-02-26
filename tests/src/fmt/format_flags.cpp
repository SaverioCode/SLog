#include <gtest/gtest.h>
#include <chrono>
#include <string>

#include <slog/core/log_record.hpp>
#include <slog/details/time.hpp>
#include <slog/fmt/format_context.hpp>
#include <slog/fmt/format_flags.hpp>


// ============================================================================
// Shared fixture — one LogRecord reused across all flag tests
// ============================================================================

class FormatFlagsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Build a deterministic timestamp: 2026-02-23 14:30:45.123456 UTC
        auto day = std::chrono::sys_days{std::chrono::year{2026}/std::chrono::month{2}/std::chrono::day{23}};
        auto tp  = day + std::chrono::hours{14} + std::chrono::minutes{30} + std::chrono::seconds{45}
                       + std::chrono::milliseconds{123} + std::chrono::microseconds{456};

        record.level         = slog::LogLevel::WARNING;
        record.logger_name   = "test_logger";
        record.string_buffer = "hello world";
        record.location      = std::source_location::current();
        record.timestamp     = tp;
        record.thread_id     = 12345;
        record.format_fn     = nullptr;
    }

    slog::LogRecord record;
    std::string dest;
};

// ---- Message & Metadata flags ----

TEST_F(FormatFlagsTest, FmtMessage)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_message(ctx, dest);
    EXPECT_EQ(dest, "hello world");
}

TEST_F(FormatFlagsTest, FmtLevelFull)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_level(ctx, dest);
    EXPECT_EQ(dest, "WARNING");
}

TEST_F(FormatFlagsTest, FmtLevelShort)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_level_short(ctx, dest);
    EXPECT_EQ(dest, "W");
}

TEST_F(FormatFlagsTest, FmtThreadId)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_thread_id(ctx, dest);
    EXPECT_EQ(dest, "12345");
}

TEST_F(FormatFlagsTest, FmtLoggerName)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_logger_name(ctx, dest);
    EXPECT_EQ(dest, "test_logger");
}

// ---- Time flags ----
// NOTE: Time flags depend on the LOCAL timezone. These tests convert
// the known UTC timestamp to local time and derive expected values,
// so they pass regardless of the test machine's timezone.

class FormatFlagsTimeTest : public FormatFlagsTest
{
protected:
    void SetUp() override
    {
        FormatFlagsTest::SetUp();

        // Derive expected local time values from the same timestamp
        std::time_t tt = std::chrono::system_clock::to_time_t(record.timestamp);
        std::tm local{};

        slog::details::localtime(&tt, &local);
        expected_year   = local.tm_year + 1900;
        expected_month  = local.tm_mon + 1;
        expected_day    = local.tm_mday;
        expected_hour   = local.tm_hour;
        expected_minute = local.tm_min;
        expected_second = local.tm_sec;
    }

    int expected_year;
    int expected_month;
    int expected_day;
    int expected_hour;
    int expected_minute;
    int expected_second;

    // Helper: format int with zero-padding to given width
    static std::string pad(int val, int width)
    {
        std::string s = std::to_string(val);
        while (static_cast<int>(s.size()) < width) {
            s.insert(s.begin(), '0');
        }
        return s;
    }
};

TEST_F(FormatFlagsTimeTest, FmtYear)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_year(ctx, dest);
    EXPECT_EQ(dest, pad(expected_year, 4));
}

TEST_F(FormatFlagsTimeTest, FmtMonth)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_month(ctx, dest);
    EXPECT_EQ(dest, pad(expected_month, 2));
}

TEST_F(FormatFlagsTimeTest, FmtDay)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_day(ctx, dest);
    EXPECT_EQ(dest, pad(expected_day, 2));
}

TEST_F(FormatFlagsTimeTest, FmtHour)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_hour(ctx, dest);
    EXPECT_EQ(dest, pad(expected_hour, 2));
}

TEST_F(FormatFlagsTimeTest, FmtMinute)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_minute(ctx, dest);
    EXPECT_EQ(dest, pad(expected_minute, 2));
}

TEST_F(FormatFlagsTimeTest, FmtSecond)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_second(ctx, dest);
    EXPECT_EQ(dest, pad(expected_second, 2));
}

TEST_F(FormatFlagsTimeTest, FmtMillisecond)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_millisecond(ctx, dest);
    EXPECT_EQ(dest, "123");
}

TEST_F(FormatFlagsTimeTest, FmtMicrosecond)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_microsecond(ctx, dest);
    EXPECT_EQ(dest, "123456");
}

// ---- Source location flags ----

TEST_F(FormatFlagsTest, FmtSourceFile)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_source_file(ctx, dest);

    // Should be just the filename, not the full path
    EXPECT_FALSE(dest.empty());
    EXPECT_EQ(dest.find('/'), std::string::npos);
    EXPECT_EQ(dest.find('\\'), std::string::npos);
    EXPECT_EQ(dest, "format_flags.cpp");
}

TEST_F(FormatFlagsTest, FmtSourcePath)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_source_path(ctx, dest);

    // Full path should be non-empty
    EXPECT_FALSE(dest.empty());
}

TEST_F(FormatFlagsTest, FmtSourceLine)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_source_line(ctx, dest);

    // Should be a non-empty numeric string
    EXPECT_FALSE(dest.empty());
    for (char c : dest) {
        EXPECT_TRUE(c >= '0' && c <= '9') << "Non-digit in line number: " << c;
    }
}

TEST_F(FormatFlagsTest, FmtSourceFunc)
{
    slog::fmt::FormatContext ctx(record);
    slog::fmt::fmt_source_func(ctx, dest);

    // Should contain the test function name
    EXPECT_FALSE(dest.empty());
    EXPECT_NE(dest.find("SetUp"), std::string::npos);
}
