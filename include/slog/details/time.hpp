#ifndef SLOG_DETAILS_TIME_HPP
#define SLOG_DETAILS_TIME_HPP

#include <ctime>
#include <chrono>

#include <slog/common.hpp>

namespace slog::details
{

struct TimeComponents
{
    int year;     // e.g. 2026
    int month;    // 1-12
    int day;      // 1-31
    int hour;     // 0-23
    int minute;   // 0-59
    int second;   // 0-59
    int millis;   // 0-999
    int micros;   // 0-999999
};

/// Thread-safe version of std::localtime
SLOG_ALWAYS_INLINE void localtime(const std::time_t* time, std::tm* result) noexcept
{
#ifdef _WIN32
    ::localtime_s(result, time);
#else
    ::localtime_r(time, result);
#endif
}

SLOG_ALWAYS_INLINE std::chrono::system_clock::duration
sub_seconds(std::chrono::system_clock::time_point tp) noexcept
{
    std::chrono::system_clock::duration epoch_dur = tp.time_since_epoch();
    std::chrono::seconds epoch_secs = std::chrono::duration_cast<std::chrono::seconds>(epoch_dur);
    return epoch_dur - epoch_secs;
}

}
#endif // SLOG_DETAILS_TIME_HPP