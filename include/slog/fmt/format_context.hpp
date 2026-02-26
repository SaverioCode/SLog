#ifndef SLOG_FMT_FORMAT_CONTEXT_HPP
#define SLOG_FMT_FORMAT_CONTEXT_HPP

#include <chrono>
#include <ctime>

#include <slog/core/log_record.hpp>
#include <slog/details/time.hpp>

namespace slog::fmt
{

class FormatContext
{
public:

    explicit FormatContext(const slog::LogRecord& rec) : record(rec) {
        _decompose(_cached_time, record.timestamp);
    }

    FormatContext(const FormatContext&) = delete;
    FormatContext(FormatContext&&) = delete;
    FormatContext& operator=(const FormatContext&) = delete;
    FormatContext& operator=(FormatContext&&) = delete;

    const slog::details::TimeComponents& time() const
    {
        return _cached_time;
    }

    const slog::LogRecord& record;

private:

    static void _decompose(slog::details::TimeComponents& tc, std::chrono::system_clock::time_point tp)
    {
        std::chrono::system_clock::duration sub_secs = slog::details::sub_seconds(tp);
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm tm_buf{};

        slog::details::localtime(&tt, &tm_buf);

        tc.year   = tm_buf.tm_year + 1900;
        tc.month  = tm_buf.tm_mon + 1;
        tc.day    = tm_buf.tm_mday;
        tc.hour   = tm_buf.tm_hour;
        tc.minute = tm_buf.tm_min;
        tc.second = tm_buf.tm_sec;
        tc.millis = static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(sub_secs).count());
        tc.micros = static_cast<int>(
            std::chrono::duration_cast<std::chrono::microseconds>(sub_secs).count());
    }

    slog::details::TimeComponents _cached_time;
};

} // namespace slog::fmt

#endif // SLOG_FMT_FORMAT_CONTEXT_HPP
