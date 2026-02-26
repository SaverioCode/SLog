#ifndef SLOG_SINKS_ISINK_HPP
#define SLOG_SINKS_ISINK_HPP

#include <iosfwd>

#include <slog/common.hpp>
#include <slog/core/log_record.hpp>
#include <slog/fmt/pattern_formatter.hpp>

namespace slog::sinks
{

class ISink
{
public:
    ISink() = delete;
    ISink(std::string_view name) : _name(name) {}
    ISink(const ISink&) = delete;
    ISink(ISink&&) = delete;
    virtual ~ISink() = default;

    ISink& operator=(const ISink&) = delete;
    ISink& operator=(ISink&&) = delete;

    virtual void flush() = 0;

    [[nodiscard]] SLOG_ALWAYS_INLINE slog::LogLevel get_level() const noexcept { return _level; }

    [[nodiscard]] SLOG_ALWAYS_INLINE const std::string& get_name() const noexcept { return _name; }

    SLOG_ALWAYS_INLINE void log(const slog::LogRecord& record, const std::string& sink_manager_pattern)
    {
        _message.clear();

        SLOG_SINK_LOCK(_sink_mutex)
        if (_formatter.get_pattern() != sink_manager_pattern) {
            _formatter.format(record, _message);
            _write(_message);
        }
        else {
            _write(record.message);
        }
    }

    SLOG_ALWAYS_INLINE void set_level(const slog::LogLevel level) noexcept { _level = level; }

    SLOG_ALWAYS_INLINE void set_pattern(std::string_view pattern) { _formatter.set_pattern(pattern); }

protected:
    virtual void _write(std::string_view message) = 0;

private:
    slog::fmt::PatternFormatter _formatter;
    std::string _name;
    std::string _message;
    slog::LogLevel _level{slog::LogLevel::TRACE};
    SLOG_SINK_MUTEX_MEMBER(_sink_mutex)
};

} // namespace slog::sinks

#endif // SLOG_SINKS_ISINK_HPP
