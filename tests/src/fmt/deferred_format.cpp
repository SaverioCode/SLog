#include <gtest/gtest.h>
#include <any>
#include <string>
#include <tuple>

#include <slog/fmt/deferred_format.hpp>

// ============================================================================
// Deferred format tests — verifies format_deferred resolves stored args
// ============================================================================

class DeferredFormatTest : public ::testing::Test
{
protected:
    std::any stored;
    std::string out;
};

TEST_F(DeferredFormatTest, SingleInt)
{
    stored.emplace<std::tuple<int>>(42);
    slog::fmt::format_deferred<int>("value={}", stored, out);
    EXPECT_EQ(out, "value=42");
}

TEST_F(DeferredFormatTest, SingleString)
{
    stored.emplace<std::tuple<std::string>>(std::string("hello"));
    slog::fmt::format_deferred<std::string>("{}", stored, out);
    EXPECT_EQ(out, "hello");
}

TEST_F(DeferredFormatTest, MixedArgs)
{
    using Tuple = std::tuple<int, std::string, double>;
    stored.emplace<Tuple>(42, std::string("world"), 3.14);
    slog::fmt::format_deferred<int, std::string, double>("{} {} {:.2f}", stored, out);
    EXPECT_EQ(out, "42 world 3.14");
}

TEST_F(DeferredFormatTest, NoArgs)
{
    stored.emplace<std::tuple<>>();
    slog::fmt::format_deferred<>("hello world", stored, out);
    EXPECT_EQ(out, "hello world");
}

TEST_F(DeferredFormatTest, FunctionPointerUsage)
{
    // Verify format_deferred can be used as a function pointer (as stored in LogRecord)
    using Tuple = std::tuple<int, const char*>;
    void (*fn)(std::string_view, std::any&, std::string&) = &slog::fmt::format_deferred<int, const char*>;

    stored.emplace<Tuple>(7, "test");
    fn("{} {}", stored, out);
    EXPECT_EQ(out, "7 test");
}

TEST_F(DeferredFormatTest, CalledTwiceOverwritesOutput)
{
    stored.emplace<std::tuple<int>>(1);
    slog::fmt::format_deferred<int>("first={}", stored, out);
    EXPECT_EQ(out, "first=1");

    stored.emplace<std::tuple<int>>(2);
    slog::fmt::format_deferred<int>("second={}", stored, out);
    EXPECT_EQ(out, "second=2");
}
