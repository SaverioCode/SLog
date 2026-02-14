#include <slog/core/logger.hpp>
#include <slog/core/registry.hpp>

namespace slog::core
{

// ------------------------
// Public methods
// ------------------------

inline std::shared_ptr<Logger>  Registry::create_logger(std::string_view name)
{
    auto new_logger = _make_logger(name);
    auto new_vec    = std::make_shared<std::vector<std::shared_ptr<Logger>>>();

    while (true) {
        LoggerVecSPtr old_vec = _loggers.load(std::memory_order_acquire);
        
        for (const auto& logger : *old_vec) {
            if (logger->get_name() == name) {
                return nullptr;
            }
        }
        *new_vec = *old_vec;
        new_vec->push_back(new_logger);
        if (_loggers.compare_exchange_strong(old_vec, new_vec, std::memory_order_acq_rel)) {
            return new_logger;
        }
    }
}

inline bool  Registry::set_default_logger_name(std::string_view name)
{
    LoggerVecSPtr loggers = _loggers.load(std::memory_order_relaxed);

    for (const auto& logger : *loggers) {
        if (logger->get_name() == name) {
            _default_logger_name = name;
            return true;
        }
    }
    return false;
}

// ------------------------
// Private methods
// ------------------------

inline Registry::Registry(LoggerState state) : _local_state(state)
{
    std::shared_ptr<Logger> logger;
    LoggerVecSPtr loggers = std::make_shared<std::vector<std::shared_ptr<Logger>>>();

    if (state == LoggerState::ACTIVE) {
        _default_logger_name = _SLOG_DEFAULT_LOGGER_NAME;
        logger = _make_logger(_default_logger_name);
        // Todo: create and add console/stdout sink
    }
    else if (state == LoggerState::INACTIVE) {
        _default_logger_name = _SLOG_INACTIVE_LOGGER_NAME;
        logger = _make_logger(_default_logger_name);
        // Todo: create and add console/stderr sink
    }
    loggers->push_back(logger);
    _loggers.store(loggers, std::memory_order_relaxed);
}

inline std::shared_ptr<Logger>  Registry::_get_logger(std::string_view name) const noexcept
{
    LoggerVecSPtr loggers = _loggers.load(std::memory_order_relaxed);

    for (auto& logger : *loggers) {
        if (logger->get_name() == name) {
            return logger;
        }
    }
    return nullptr;
}

SLOG_FORCE_INLINE std::shared_ptr<Logger>  Registry::_make_logger(std::string_view name)
{
    struct TmpLogger : public Logger { TmpLogger(std::string_view name) : Logger(name) {} };
    return std::make_shared<TmpLogger>(name);
}

SLOG_FORCE_INLINE std::shared_ptr<Registry>  Registry::_make_registry(LoggerState state)
{
    struct TmpRegistry : public Registry { TmpRegistry(LoggerState state) : Registry(state) {} };
    return std::make_shared<TmpRegistry>(state);
}

}
