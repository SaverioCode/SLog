#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <slog/sinks/isink.hpp>

namespace slog::tests 
{

class AsyncVectorSink : public slog::sinks::ISink 
{
public:
    AsyncVectorSink(std::string_view name) : ISink(name) {}

    AsyncVectorSink(const AsyncVectorSink&) = delete;
    AsyncVectorSink(AsyncVectorSink&&) = delete;
    ~AsyncVectorSink() override = default;
    AsyncVectorSink& operator=(const AsyncVectorSink&) = delete;
    AsyncVectorSink& operator=(AsyncVectorSink&&) = delete;
    
    bool wait_for(size_t count, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) 
    {
        std::unique_lock<std::mutex> lock(_mutex);
        return _cv.wait_for(lock, timeout, [this, count] { 
            return messages.size() >= count; 
        });
    }

    void clear() {
        std::lock_guard<std::mutex> lock(_mutex);
        messages.clear();
    }

    std::string get(size_t index) {
        std::lock_guard<std::mutex> lock(_mutex);
        return messages[index];
    }

    size_t buffer_size() {
        std::lock_guard<std::mutex> lock(_mutex);
        return messages.size();
    }

    void flush() override {}
    
private:
    void _write(std::string_view message) override 
    {
        std::lock_guard<std::mutex> lock(_mutex);
        messages.push_back(std::string(message));
        _cv.notify_one();
    }
    
    std::vector<std::string> messages;
    std::mutex _mutex;
    std::condition_variable _cv;
};

} // namespace slog::tests
