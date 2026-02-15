#ifndef SLOG_SINKS_SINK_MANAGER_HPP
#define SLOG_SINKS_SINK_MANAGER_HPP

#include <atomic>
#include <memory>
#include <vector>

#include <slog/sinks/isink.hpp>

namespace slog::sinks
{

class SinkManager
{
    using SinkVec = std::vector<std::shared_ptr<ISink>>;
    using SinkPtr = std::shared_ptr<SinkVec>;
    
    public:
        SinkManager() : _sinks_ptr(std::make_shared<SinkVec>()) {}
        SinkManager(std::shared_ptr<ISink> sink) : _sinks_ptr(std::make_shared<SinkVec>()) { addSink(sink); }
        SinkManager(SinkVec sinks) : _sinks_ptr(std::make_shared<SinkVec>(sinks)) {}
        SinkManager(const SinkManager&) = delete;
        SinkManager(SinkManager&&) = delete;

        SinkManager& operator=(const SinkManager&) = delete;
        SinkManager& operator=(SinkManager&&) = delete;
        
        bool    addSink(std::shared_ptr<ISink> sink)
        {
            SLOG_LOCK(_sink_manager_mutex)
            auto name = sink->getName();
            auto current_sinks = _sinks_ptr.load(std::memory_order_relaxed);
            SinkPtr new_sinks;

            for (const auto& existing_sink : *current_sinks) {
                if (existing_sink->getName() == name) {
                    return false;
                }
            }
            new_sinks = std::make_shared<SinkVec>(*current_sinks);
            new_sinks->push_back(sink);
            _sinks_ptr.store(new_sinks, std::memory_order_release);
            return true;
        }

        void    dispatch(const slog::LogRecord& record)
        {
            auto current_sinks = _sinks_ptr.load(std::memory_order_acquire);

            for (auto& sink : *current_sinks) {
                if (record.level <= sink->getLevel()) {
                    sink->log(record);
                }
            }
        }

        [[nodiscard]] std::shared_ptr<ISink>  getSink(const std::string& name)
        {
            auto current_sinks = _sinks_ptr.load(std::memory_order_acquire);

            for (const auto& sink : *current_sinks) {
                if (sink->getName() == name) {
                    return sink;
                }
            }
            return nullptr;
        }

        [[nodiscard]] SLOG_ALWAYS_INLINE SinkPtr getSinkList() const noexcept
        {
            return _sinks_ptr.load(std::memory_order_acquire);
        }

        void    removeSink(const std::string& name)
        {
            SLOG_LOCK(_sink_manager_mutex)
            auto current_sinks = _sinks_ptr.load(std::memory_order_relaxed);

            for (auto it = current_sinks->begin(); it != current_sinks->end(); it++) {
                if ((*it)->getName() == name) {
                    auto new_sinks = std::make_shared<SinkVec>(*current_sinks);

                    new_sinks->erase(std::next(new_sinks->begin(), std::distance(current_sinks->begin(), it)));
                    _sinks_ptr.store(new_sinks, std::memory_order_release);
                    return;
                }
            }
        }

    private:
        std::atomic<SinkPtr> _sinks_ptr;
        SLOG_MUTEX_MEMBER(_sink_manager_mutex)
};

}

#endif // SLOG_SINKS_SINK_MANAGER_HPP
