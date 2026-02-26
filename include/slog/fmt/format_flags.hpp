#ifndef SLOG_FMT_FORMAT_FLAGS_HPP
#define SLOG_FMT_FORMAT_FLAGS_HPP

#include <charconv>
#include <limits>
#include <string>
#include <string_view>

#include <slog/common.hpp>
#include <slog/core/log_level.hpp>
#include <slog/core/log_record.hpp>
#include <slog/details/thread_id.hpp>
#include <slog/details/filesystem.hpp>
#include <slog/fmt/format_context.hpp>

namespace slog::fmt
{

using FormatFn = void(*)(const FormatContext& ctx, std::string& dest);

namespace detail
{

inline void append_padded(std::string& dest, int value, int width)
{
    static constexpr size_t MAX_DIGITS = std::numeric_limits<int>::digits10 + 1;
    char buf[MAX_DIGITS];
    std::to_chars_result result;
    int digits;
    
    result = std::to_chars(buf, buf + sizeof(buf), value);
    if (result.ec != std::errc()) {
        return;
    }

    digits = static_cast<int>(result.ptr - buf);
    while (digits < width) {
        dest.push_back('0');
        digits++;
    }
    dest.append(buf, result.ptr);
}

} // namespace detail

// %v — user message
SLOG_ALWAYS_INLINE void fmt_message(const FormatContext& ctx, std::string& dest)
{
    dest.append(ctx.record.string_buffer);
}

// %l — full level name
SLOG_ALWAYS_INLINE void fmt_level(const FormatContext& ctx, std::string& dest)
{
    dest.append(slog::to_string(ctx.record.level));
}

// %L — short level (single char)
SLOG_ALWAYS_INLINE void fmt_level_short(const FormatContext& ctx, std::string& dest)
{
    dest.push_back(slog::to_string(ctx.record.level)[0]);
}

// %t — thread id (native OS thread ID)
inline void fmt_thread_id(const FormatContext& ctx, std::string& dest)
{
    static constexpr size_t MAX_DIGITS = std::numeric_limits<size_t>::digits10 + 1;
    char buf[MAX_DIGITS];

    std::to_chars_result result = std::to_chars(buf, buf + sizeof(buf), ctx.record.thread_id);
    dest.append(buf, result.ptr);
}

// %Y — year (4 digits)
SLOG_ALWAYS_INLINE void fmt_year(const FormatContext& ctx, std::string& dest)
{
    detail::append_padded(dest, ctx.time().year, 4);
}

// %m — month (01-12)
SLOG_ALWAYS_INLINE void fmt_month(const FormatContext& ctx, std::string& dest)
{
    detail::append_padded(dest, ctx.time().month, 2);
}

// %d — day (01-31)
SLOG_ALWAYS_INLINE void fmt_day(const FormatContext& ctx, std::string& dest)
{
    detail::append_padded(dest, ctx.time().day, 2);
}

// %H — hour (00-23)
SLOG_ALWAYS_INLINE void fmt_hour(const FormatContext& ctx, std::string& dest)
{
    detail::append_padded(dest, ctx.time().hour, 2);
}

// %M — minute (00-59)
SLOG_ALWAYS_INLINE void fmt_minute(const FormatContext& ctx, std::string& dest)
{
    detail::append_padded(dest, ctx.time().minute, 2);
}

// %S — second (00-59)
SLOG_ALWAYS_INLINE void fmt_second(const FormatContext& ctx, std::string& dest)
{
    detail::append_padded(dest, ctx.time().second, 2);
}

// %e — milliseconds (000-999)
SLOG_ALWAYS_INLINE void fmt_millisecond(const FormatContext& ctx, std::string& dest)
{
    detail::append_padded(dest, ctx.time().millis, 3);
}

// %f — microseconds (000000-999999)
SLOG_ALWAYS_INLINE void fmt_microsecond(const FormatContext& ctx, std::string& dest)
{
    detail::append_padded(dest, ctx.time().micros, 6);
}

// %s — source filename (basename only)
SLOG_ALWAYS_INLINE void fmt_source_file(const FormatContext& ctx, std::string& dest)
{
    const char* file = ctx.record.location.file_name();
    dest.append(slog::details::path_basename(file ? std::string_view(file) : std::string_view{}));
}

// %g — source full path
SLOG_ALWAYS_INLINE void fmt_source_path(const FormatContext& ctx, std::string& dest)
{
    const char* file = ctx.record.location.file_name();
    if (file) {
        dest.append(file);
    }
}

// %# — source line number
inline void fmt_source_line(const FormatContext& ctx, std::string& dest)
{
    static constexpr size_t MAX_DIGITS = std::numeric_limits<std::uint_least32_t>::digits10 + 1;
    char buf[MAX_DIGITS];

    std::to_chars_result result = std::to_chars(buf, buf + sizeof(buf), ctx.record.location.line());
    if (result.ec != std::errc()) {
        return;
    }
    dest.append(buf, result.ptr);
}

// %! — source function name
SLOG_ALWAYS_INLINE void fmt_source_func(const FormatContext& ctx, std::string& dest)
{
    const char* func = ctx.record.location.function_name();
    if (func) {
        dest.append(func);
    }
}

// %n — logger name (placeholder until logger_name is added to LogRecord)
SLOG_ALWAYS_INLINE void fmt_logger_name(const FormatContext& ctx, std::string& dest)
{
    dest.append(ctx.record.logger_name);
}

} // namespace slog::fmt

#endif // SLOG_FMT_FORMAT_FLAGS_HPP
