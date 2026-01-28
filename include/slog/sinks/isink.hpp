#ifndef SLOG_SINKS_ISINK_HPP
#define SLOG_SINKS_ISINK_HPP

#include <string>

#include <slog/config.hpp>
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

        virtual void    flush() = 0;

        [[nodiscard]] SLOG_FORCE_INLINE slog::core::LogLevel    getLevel() const noexcept
        {
            return _level;
        }

        [[nodiscard]] SLOG_FORCE_INLINE const std::string&  getName() const noexcept
        {
            return _name;
        }

        SLOG_FORCE_INLINE void  log(const slog::core::LogRecord& record)
        {
            SLOG_SINK_LOCK(_sink_mutex)
            _write(record);
        }

        SLOG_FORCE_INLINE void  setLevel(const slog::core::LogLevel level) noexcept
        {
            _level = level;
        }
    
    protected:
        virtual void    _write(const slog::core::LogRecord& record) = 0;

    private:
        std::string             _name;
        slog::core::LogLevel    _level{slog::core::LogLevel::TRACE};
        SLOG_SINK_MUTEX_MEMBER(_sink_mutex)
};

}

#endif // SLOG_SINKS_ISINK_HPP
