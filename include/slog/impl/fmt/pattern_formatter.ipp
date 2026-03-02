#ifndef SLOG_IMPL_FMT_PATTERN_FORMATTER_HPP
#define SLOG_IMPL_FMT_PATTERN_FORMATTER_HPP

#include <slog/details/macros.hpp>
#include <slog/fmt/pattern_formatter.hpp>

namespace slog::fmt
{

    SLOG_API std::array<FormatFn, PatternFormatter::TABLE_SIZE> PatternFormatter::_dispatch_table = [] {
        std::array<FormatFn, PatternFormatter::TABLE_SIZE> t{};

        t.fill(nullptr);
        t['v'] = fmt_message;
        t['l'] = fmt_level;
        t['L'] = fmt_level_short;
        t['t'] = fmt_thread_id;
        t['Y'] = fmt_year;
        t['m'] = fmt_month;
        t['d'] = fmt_day;
        t['H'] = fmt_hour;
        t['M'] = fmt_minute;
        t['S'] = fmt_second;
        t['e'] = fmt_millisecond;
        t['f'] = fmt_microsecond;
        t['s'] = fmt_source_file;
        t['g'] = fmt_source_path;
        t['#'] = fmt_source_line;
        t['!'] = fmt_source_func;
        t['n'] = fmt_logger_name;
        return t;
    }();


}

#endif // SLOG_IMPL_FMT_PATTERN_FORMATTER_HPP