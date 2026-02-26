#ifndef SLOG_SINKS_SINK_MANAGER_HPP
#define SLOG_SINKS_SINK_MANAGER_HPP

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

#include <slog/sinks/isink.hpp>
#include <slog/fmt/pattern_formatter.hpp>

namespace slog::sinks
{

class SinkManager
{
    using SinkVec = std::vector<std::shared_ptr<ISink>>;
    using SinkVecPtr = std::shared_ptr<SinkVec>;

private:

    template <typename F>
    SLOG_ALWAYS_INLINE decltype(auto) _guard(F&& func) const noexcept
    {
#if defined(SLOG_ASYNC_ENABLED) || defined(SLOG_TSAFE_ENABLED)
        while (_flag.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
#endif

        if constexpr (std::is_void_v<std::invoke_result_t<F>>) {
            func();
#if defined(SLOG_ASYNC_ENABLED) || defined(SLOG_TSAFE_ENABLED)
            _flag.clear(std::memory_order_release);
#endif
        }
        else {
            auto result = func();
#if defined(SLOG_ASYNC_ENABLED) || defined(SLOG_TSAFE_ENABLED)
            _flag.clear(std::memory_order_release);
#endif
            return result;
        }
    }

public:
    SinkManager() = default;

    SinkManager(std::shared_ptr<ISink> sink)
    {
        add_sink(sink);
    }

    SinkManager(SinkVec sinks)
    {
        for (auto& sink : sinks) {
            add_sink(sink);
        }
    }

    SinkManager(const SinkManager&) = delete;
    SinkManager(SinkManager&&) = delete;

    SinkManager& operator=(const SinkManager&) = delete;
    SinkManager& operator=(SinkManager&&) = delete;

    bool add_sink(std::shared_ptr<ISink> sink)
    {
        return _guard([&]()
        {
            std::string name = sink->get_name();

            for (const std::shared_ptr<ISink>& existing_sink : _sinks_vec) {
                if (existing_sink->get_name() == name) {
                    return false;
                }
            }
            _sinks_vec.push_back(sink);
            return true;
        });
    }

    void dispatch(slog::LogRecord& record)
    {
        if (record.format_fn) {
            record.format_fn(record.format_str, record.stored_args, record.string_buffer);
        }
        _guard([&]()
        {
            _formatter.format(record, record.message);
            for (const std::shared_ptr<ISink>& sink : _sinks_vec) {
                if (record.level <= sink->get_level()) {
                    sink->log(record, _formatter.get_pattern());
                }
            }
        });
    }

    void flush() const
    {
        _guard([&]()
        {
            for (const std::shared_ptr<ISink>& sink : _sinks_vec) {
                sink->flush();
            }
        });
    }

    [[nodiscard]] std::shared_ptr<ISink> get_sink(const std::string& name) const noexcept
    {
        return _guard([&]()
        {
            for (const std::shared_ptr<ISink>& sink : _sinks_vec) {
                if (sink->get_name() == name) {
                    return sink;
                }
            }
            return std::shared_ptr<ISink>{nullptr};
        });
    }

    [[nodiscard]] SLOG_ALWAYS_INLINE SinkVecPtr get_sinks() const noexcept
    {
        return _guard([&]()
        {
            return std::make_shared<std::vector<std::shared_ptr<ISink>>>(_sinks_vec);
        });
    }

    [[nodiscard]] bool has_sink(const std::string& name) const noexcept
    {
        return _guard([&]()
        {
            for (const std::shared_ptr<ISink>& sink : _sinks_vec) {
                if (sink->get_name() == name) {
                    return true;
                }
            }
            return false;
        });
    }

    void remove_sink(const std::string& name)
    {
        _guard([&]()
        {
            for (auto it = _sinks_vec.begin(); it != _sinks_vec.end(); it++) {
                if ((*it)->get_name() == name) {
                    _sinks_vec.erase(it);
                    return;
                }
            }
        });
    }

    void set_pattern(std::string_view pattern)
    {
        _guard([&]()
        {
            _formatter.set_pattern(pattern);
            for (const std::shared_ptr<ISink>& sink : _sinks_vec) {
                sink->set_pattern(pattern);
            }
        });
    }

private:

    slog::fmt::PatternFormatter _formatter;
    std::vector<std::shared_ptr<ISink>> _sinks_vec;
    mutable std::atomic_flag _flag;
};

} // namespace slog::sinks

#endif // SLOG_SINKS_SINK_MANAGER_HPP
