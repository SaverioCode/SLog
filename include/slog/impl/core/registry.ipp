#ifndef SLOG_CORE_REGISTRY_IPP
#define SLOG_CORE_REGISTRY_IPP

#include <cstdio>

#include <slog/core/logger.hpp>
#include <slog/core/registry.hpp>
#include <slog/sinks/console_sink.hpp>

namespace slog
{

// ------------------------
// Public methods
// ------------------------

SLOG_INLINE std::shared_ptr<Logger> Registry::create_logger(std::string_view name)
{
    std::shared_ptr<Logger> new_logger{nullptr};
    SLOG_LOCK(_mutex);

    for (auto& logger : _loggers) {
        if (logger->get_name() == name) {
            return logger;
        }
    }
    new_logger = _make_logger(name, _worker);
    _loggers.push_back(new_logger);
    return new_logger;
}

SLOG_INLINE void Registry::flush() const
{
    SLOG_LOCK(_mutex);

    for (auto& logger : _loggers) {
        logger->flush();
    }
}

SLOG_INLINE bool Registry::set_default_logger_name(std::string_view name)
{
    SLOG_LOCK(_mutex);

    for (const auto& logger : _loggers) {
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

SLOG_INLINE Registry::Registry()
{
    std::shared_ptr<Logger> logger;
    _local_state = _state.load(std::memory_order_relaxed);

#ifdef SLOG_ASYNC_ENABLED
    _worker = std::make_shared<slog::async::Worker>();
#endif

    if (_local_state == RegistryState::ACTIVE) {
        _default_logger_name = _SLOG_DEFAULT_LOGGER_NAME;
        logger = _make_logger(_default_logger_name, _worker);
        logger->add_sink(
            std::make_shared<slog::sinks::ConsoleSink>(_SLOG_DEFAULT_SINK_NAME, stdout));
    }
    else if (_local_state == RegistryState::INACTIVE) {
        _default_logger_name = _SLOG_INACTIVE_LOGGER_NAME;
        logger = _make_logger(_default_logger_name, _worker);
        logger->add_sink(
            std::make_shared<slog::sinks::ConsoleSink>(_SLOG_INACTIVE_SINK_NAME, stderr));
    }
    _loggers.push_back(logger);
}

SLOG_INLINE Logger& Registry::_get_logger(std::string_view name) const noexcept
{
    {
        SLOG_LOCK(_mutex);

        for (auto& logger : _loggers) {
            if (logger->get_name() == name) {
                return *(logger.get());
            }
        }
    }
    return this->get_default_logger();
}

SLOG_INLINE std::shared_ptr<Logger> Registry::_get_logger_ptr(std::string_view name) const noexcept
{
    {
        SLOG_LOCK(_mutex);

        for (auto& logger : _loggers) {
            if (logger->get_name() == name) {
                return logger;
            }
        }
    }
    return nullptr;
}

SLOG_ALWAYS_INLINE std::shared_ptr<Logger>
Registry::_make_logger(std::string_view name, std::shared_ptr<slog::async::Worker> worker)
{
    struct TmpLogger : public Logger
    {
        TmpLogger(std::string_view name, std::shared_ptr<slog::async::Worker> worker)
            : Logger(name, worker)
        {
        }
    };
    return std::make_shared<TmpLogger>(name, worker);
}

} // namespace slog

#endif // SLOG_CORE_REGISTRY_IPP
