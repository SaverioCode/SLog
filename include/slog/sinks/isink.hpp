#ifndef SLOG_SINKS_ISINK_HPP
#define SLOG_SINKS_ISINK_HPP

#include <iosfwd>

#include <slog/common.hpp>
#include <slog/core/log_record.hpp>

namespace slog::sinks
{

class ISink
{
public:
    ISink() = delete;
    ISink(const std::string& name) : _name(name) {}
    ISink(const ISink&) = default;
    ISink(ISink&&) = default;
    virtual ~ISink() = default;

    ISink& operator=(const ISink&) = default;
    ISink& operator=(ISink&&) = default;

    virtual void flush() = 0;

    [[nodiscard]] SLOG_ALWAYS_INLINE slog::LogLevel get_level() const noexcept { return _level; }

    [[nodiscard]] SLOG_ALWAYS_INLINE const std::string& get_name() const noexcept { return _name; }

    SLOG_ALWAYS_INLINE void log(const slog::LogRecord& record)
    {
        SLOG_SINK_LOCK(_sink_mutex)
        _write(record);
    }

    SLOG_ALWAYS_INLINE void set_level(const slog::LogLevel level) noexcept { _level = level; }

protected:
    virtual void _write(const slog::LogRecord& record) = 0;

private:
    std::string _name;
    slog::LogLevel _level{slog::LogLevel::TRACE};
    SLOG_SINK_MUTEX_MEMBER(_sink_mutex)
};

} // namespace slog::sinks

#endif // SLOG_SINKS_ISINK_HPP
