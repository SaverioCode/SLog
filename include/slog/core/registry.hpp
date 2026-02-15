#ifndef SLOG_CORE_REGISTRY_HPP
#define SLOG_CORE_REGISTRY_HPP

#include <slog/config.hpp>
#include <slog/core/log_level.hpp>

// ------------------------
// Forward declarations
// ------------------------

namespace slog
{
    class Logger;
}

namespace slog
{

class Registry
{
    public:
        ~Registry()
        {
            if (_state.load(std::memory_order_relaxed) == LoggerState::ACTIVE) {
                _state.store(LoggerState::INACTIVE);
            }
            _loggers.load(std::memory_order_relaxed)->clear();
        }

        static std::shared_ptr<Registry> instance()
        {
            if (_state.load(std::memory_order_relaxed) == LoggerState::ACTIVE) [[likely]] {
                static std::shared_ptr<Registry> instance = _make_registry(LoggerState::ACTIVE);
                return instance;
            }
            if (_state.load(std::memory_order_relaxed) == LoggerState::INACTIVE) [[unlikely]] {
                static std::shared_ptr<Registry> instance = _make_registry(LoggerState::INACTIVE);
                return instance;   
            }
            return nullptr;
        }

        [[nodiscard]] std::shared_ptr<Logger> get_default_logger() const noexcept
        {
            return _get_logger(_default_logger_name);
        }

        [[nodiscard]] std::shared_ptr<Logger> get_logger(std::string_view name) const noexcept
        {
            if (_local_state == LoggerState::ACTIVE) [[likely]] {
                return this->_get_logger(name);
            }
            else if (_local_state == LoggerState::INACTIVE) [[unlikely]] {
                return this->_get_logger(_default_logger_name);
            }
            return nullptr;
        }

        [[nodiscard]] SLOG_ALWAYS_INLINE LogLevel  get_log_level() const noexcept
        {
            return _log_level;
        }

        SLOG_ALWAYS_INLINE void  set_log_level(const LogLevel level) noexcept
        {
            _log_level = level;
        }

        bool  set_default_logger_name(std::string_view name);

        [[nodiscard]] std::shared_ptr<Logger> create_logger(std::string_view name);


    private:
        using LoggerVecSPtr = std::shared_ptr<std::vector<std::shared_ptr<Logger>>>;

        enum class LoggerState
        {
            ACTIVE,
            INACTIVE
        };

        std::string_view  to_string(LoggerState state)
        {
            switch (state)
            {
                case LoggerState::ACTIVE:   return "ACTIVE";
                case LoggerState::INACTIVE: return "INACTIVE";
                default:                    return "UNKNOWN";
            }
        }

        Registry(LoggerState state);
        Registry(const Registry&) = delete;
        Registry(Registry&&) = delete;

        Registry& operator=(const Registry&) = delete;
        Registry& operator=(Registry&&) = delete;

        std::shared_ptr<Logger>  _get_logger(std::string_view name) const noexcept;
        std::shared_ptr<Logger>  _make_logger(std::string_view name);
        static std::shared_ptr<Registry>  _make_registry(LoggerState state);

        std::string                             _default_logger_name;
        LoggerState                             _local_state;
        std::atomic<LoggerVecSPtr>              _loggers{nullptr};
        static inline std::atomic<LoggerState>  _state{LoggerState::ACTIVE};
        static inline LogLevel                  _log_level{LogLevel::TRACE};
};

}

#endif // SLOG_CORE_REGISTRY_HPP