#ifndef SLOG_CORE_REGISTRY_HPP
#define SLOG_CORE_REGISTRY_HPP

#include <slog/async/worker.hpp>
#include <slog/common.hpp>
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
        if (_state.load(std::memory_order_relaxed) == RegistryState::ACTIVE) {
            _state.store(RegistryState::INACTIVE);
        }
        _loggers.load(std::memory_order_relaxed)->clear();
    }

    [[nodiscard]] std::shared_ptr<Logger> create_logger(std::string_view name);

    static std::shared_ptr<Registry> instance()
    {
        if (_state.load(std::memory_order_relaxed) == RegistryState::ACTIVE) [[likely]] {
            static std::shared_ptr<Registry> instance = _make_registry(RegistryState::ACTIVE);
            return instance;
        }
        if (_state.load(std::memory_order_relaxed) == RegistryState::INACTIVE) [[unlikely]] {
            static std::shared_ptr<Registry> instance = _make_registry(RegistryState::INACTIVE);
            return instance;
        }
        return nullptr;
    }

    void flush() const;

    [[nodiscard]] std::shared_ptr<Logger> get_default_logger() const noexcept
    {
        return _get_logger(_default_logger_name);
    }

    [[nodiscard]] std::shared_ptr<Logger> get_logger(std::string_view name) const noexcept
    {
        if (_local_state == RegistryState::ACTIVE) [[likely]] {
            return this->_get_logger(name);
        }
        else if (_local_state == RegistryState::INACTIVE) [[unlikely]] {
            return this->_get_logger(_default_logger_name);
        }
        return nullptr;
    }

    [[nodiscard]] SLOG_ALWAYS_INLINE std::shared_ptr<std::vector<std::shared_ptr<Logger>>>
    get_logger_list() const noexcept
    {
        return _loggers.load(std::memory_order_acquire);
    }

    [[nodiscard]] SLOG_ALWAYS_INLINE LogLevel get_log_level() const noexcept { return _log_level; }

    SLOG_ALWAYS_INLINE void set_log_level(const LogLevel level) noexcept { _log_level = level; }

    bool set_default_logger_name(std::string_view name);

private:
    using LoggerVecSPtr = std::shared_ptr<std::vector<std::shared_ptr<Logger>>>;

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

    Registry() = delete;
    Registry(RegistryState state);
    Registry(const Registry&) = delete;
    Registry(Registry&&) = delete;

    Registry& operator=(const Registry&) = delete;
    Registry& operator=(Registry&&) = delete;

    std::shared_ptr<Logger> _get_logger(std::string_view name) const noexcept;
    std::shared_ptr<Logger> _make_logger(std::string_view name,
                                         std::shared_ptr<slog::async::Worker> worker);
    static std::shared_ptr<Registry> _make_registry(RegistryState state);

    std::string _default_logger_name;
    RegistryState _local_state;
    std::atomic<LoggerVecSPtr> _loggers{nullptr};
    static inline std::atomic<RegistryState> _state{RegistryState::ACTIVE};
    static inline LogLevel _log_level{LogLevel::TRACE};
    std::shared_ptr<Worker> _worker{nullptr};
};

} // namespace slog

#endif // SLOG_CORE_REGISTRY_HPP