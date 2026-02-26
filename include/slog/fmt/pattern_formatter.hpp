#ifndef SLOG_FMT_PATTERN_FORMATTER_HPP
#define SLOG_FMT_PATTERN_FORMATTER_HPP

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <slog/fmt/format_context.hpp>
#include <slog/fmt/format_flags.hpp>

namespace slog::fmt
{

class PatternFormatter
{
    static constexpr std::string_view DEFAULT_PATTERN = "[%Y-%m-%d %H:%M:%S.%e] [%l] %v\n";
    static constexpr size_t TABLE_SIZE = 128;

public:

    PatternFormatter()
    {
        set_pattern(DEFAULT_PATTERN);
    }

    explicit PatternFormatter(std::string_view pattern)
    {
        set_pattern(pattern);
    }

    PatternFormatter(const PatternFormatter&) = delete;
    PatternFormatter(PatternFormatter&&) = delete;
    ~PatternFormatter() = default;

    PatternFormatter& operator=(const PatternFormatter&) = delete;
    PatternFormatter& operator=(PatternFormatter&&) = delete;

    void set_pattern(std::string_view pattern)
    {
        _pattern = pattern;
        _steps.clear();
        _compile(pattern);
    }

    [[nodiscard]] const std::string& get_pattern() const noexcept
    {
        return _pattern;
    }

    void format(const slog::LogRecord& record, std::string& dest) const
    {
        FormatContext ctx(record);

        for (const FormatStep& step : _steps) {
            if (step.flag_char == '\0') {
                dest.append(step.literal);
            }
            else {
                _dispatch_table[static_cast<uint8_t>(step.flag_char)](ctx, dest);
            }
        }
    }

    static void register_flag(char c, FormatFn fn)
    {
        uint8_t idx = static_cast<uint8_t>(c);

        if (idx < TABLE_SIZE) {
            _dispatch_table[idx] = fn;
        }
    }

private:

    struct FormatStep
    {
        char flag_char;        // '\0' for literal, otherwise the flag character
        std::string literal;   // only populated when flag_char == '\0'
    };

    void _compile(std::string_view pattern)
    {
        static std::string literal_accum;

        for (size_t i = 0; i < pattern.size();) {
            if (pattern[i] == '%' && i + 1 < pattern.size()) {
                char next = pattern[i + 1];

                if (next == '%') {
                    // %% → literal '%'
                    literal_accum.push_back('%');
                    i += 2;
                    continue;
                }

                uint8_t idx = static_cast<uint8_t>(next);
                if (idx < TABLE_SIZE && _dispatch_table[idx] != nullptr) {
                    _literal_flush(literal_accum);
                    _steps.push_back(FormatStep{next, {}});
                }
                else {
                    // Unknown flag → treat as literal "%X"
                    literal_accum.push_back('%');
                    literal_accum.push_back(next);
                }

                i += 2;
            }
            else {
                literal_accum.push_back(pattern[i]);
                i++;
            }
        }

        _literal_flush(literal_accum);
    }

    SLOG_ALWAYS_INLINE void _literal_flush(std::string& literal_accum)
    {
        if (!literal_accum.empty()) {
            _steps.push_back(FormatStep{'\0', std::move(literal_accum)});
            literal_accum.clear();
        }
    }

    static inline std::array<FormatFn, TABLE_SIZE> _dispatch_table = [] {
        std::array<FormatFn, TABLE_SIZE> t{};

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

    std::vector<FormatStep> _steps;
    std::string _pattern;
};

} // namespace slog::fmt

#endif // SLOG_FMT_PATTERN_FORMATTER_HPP
