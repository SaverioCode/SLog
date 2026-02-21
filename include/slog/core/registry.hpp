#ifndef SLOG_CORE_REGISTRY_HPP
#define SLOG_CORE_REGISTRY_HPP

#include <slog/async/worker.hpp>
#include <slog/common.hpp>
#include <slog/core/log_level.hpp>

#include <atomic>

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
        if (_state.load(std::memory_order_relaxed) == RegistryState::ACTIVE) {
            _state.store(RegistryState::INACTIVE);
        }
        _loggers.clear();
    }

    [[nodiscard]] std::shared_ptr<Logger> create_logger(std::string_view name);

    [[nodiscard]] static Registry& instance()
    {
        static Registry instance = Registry();
        return instance;
    }

    void flush() const;

    [[nodiscard]] SLOG_ALWAYS_INLINE Logger& get_default_logger() const noexcept
    {
        return this->_get_logger(_default_logger_name);
    }

    [[nodiscard]] SLOG_ALWAYS_INLINE std::shared_ptr<Logger> get_default_logger_ptr() const noexcept
    {
        return this->_get_logger_ptr(_default_logger_name);
    }

    // `get_logger` if the logger is not found it will return the default logger
    // this is done to guarantee no throws and no undefined behavior
    // being the default logger always existing it is safe to return it
    // this method is intended to be used for hot-path while `get_logger_ptr` is for cold-path
    [[nodiscard]] SLOG_ALWAYS_INLINE Logger& get_logger(std::string_view name) const noexcept
    {
        return this->_get_logger(name);
    }

    // `get_logger_ptr` can return nullptr if the logger doesn't exist
    // it's intended to be used when a Logger class member is required
    [[nodiscard]] std::shared_ptr<Logger> get_logger_ptr(std::string_view name) const noexcept
    {
        if (_local_state == RegistryState::ACTIVE) [[likely]] {
            return this->_get_logger_ptr(name);
        }
        if (_local_state == RegistryState::INACTIVE) [[unlikely]] {
            return this->_get_logger_ptr(_default_logger_name);
        }
        return nullptr;
    }

    [[nodiscard]] SLOG_ALWAYS_INLINE LogLevel get_log_level() const noexcept { return _log_level; }

    SLOG_ALWAYS_INLINE void set_log_level(const LogLevel level) noexcept { _log_level = level; }

    bool set_default_logger_name(std::string_view name);

private:
    enum class RegistryState
    {
        ACTIVE,
        INACTIVE
    };

    std::string_view to_string(RegistryState state)
    {
        switch (state) {
        case RegistryState::ACTIVE: return "ACTIVE";
        case RegistryState::INACTIVE: return "INACTIVE";
        default: return "UNKNOWN";
        }
    }

    Registry();
    Registry(const Registry&) = delete;
    Registry(Registry&&) = delete;

    Registry& operator=(const Registry&) = delete;
    Registry& operator=(Registry&&) = delete;

    [[nodiscard]] Logger& _get_logger(std::string_view name) const noexcept;
    [[nodiscard]] std::shared_ptr<Logger> _get_logger_ptr(std::string_view name) const noexcept;
    std::shared_ptr<Logger> _make_logger(std::string_view name,
                                         std::shared_ptr<slog::async::Worker> worker);

    std::string _default_logger_name;
    RegistryState _local_state;
    std::vector<std::shared_ptr<Logger>> _loggers;
    std::shared_ptr<slog::async::Worker> _worker{nullptr};
    static inline std::atomic<RegistryState> _state{RegistryState::ACTIVE};
    static inline LogLevel _log_level{LogLevel::TRACE};
    SLOG_MUTEX_MEMBER(_mutex);
};

} // namespace slog

#endif // SLOG_CORE_REGISTRY_HPP