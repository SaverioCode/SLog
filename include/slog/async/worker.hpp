#ifndef SLOG_ASYNC_WORKER_HPP
#define SLOG_ASYNC_WORKER_HPP

#ifdef SLOG_ASYNC_ENABLED

    #include <atomic>
    #include <new>
    #include <thread>

    #include <slog/async/async_op.hpp>
    #include <slog/async/mpsc_queue.hpp>
    #include <slog/core/common.hpp>
    #include <slog/sinks/sink_manager.hpp>

namespace slog::async
{

class Worker
{
public:
    Worker() : _running(true), _worker_thread(&Worker::_loop, this) {}

    Worker(const Worker&) = delete;
    Worker(Worker&&) = delete;

    Worker& operator=(const Worker&) = delete;
    Worker& operator=(Worker&&) = delete;

    ~Worker()
    {
        stop();
        join();
    }

    SLOG_ALWAYS_INLINE bool push(AsyncOp&& op)
    {
        bool ret = _queue.push(std::move(op));

        if (ret && !_flag.exchange(true, std::memory_order_release)) {
            _flag.notify_one();
        }
        return ret;
    }

    SLOG_ALWAYS_INLINE void stop() { _running.store(false, std::memory_order_release); }

    SLOG_ALWAYS_INLINE void join()
    {
        if (_worker_thread.joinable()) {
            _worker_thread.join();
        }
    }

private:
    void _loop()
    {
        AsyncOp op;

        while (_running.load(std::memory_order_relaxed)) {
            while (_queue.pop(op)) {
                op.sink_manager->dispatch(op.record);
            }

            _flag.store(false, std::memory_order_release);
            if (!_queue.pop(op)) {
                _flag.wait(false, std::memory_order_acquire);
            }
            else {
                op.sink_manager->dispatch(op.record);
            }
        }

        while (_queue.pop(op)) {
            op.sink_manager->dispatch(op.record);
        }
    }

    MPSCQueue<AsyncOp, SLOG_MPSC_QUEUE_SIZE, BlockOnFull> _queue;
    std::thread _worker_thread;
    alignas(SLOG_CACHELINE_SIZE) std::atomic<bool> _running;
    alignas(SLOG_CACHELINE_SIZE) std::atomic<bool> _flag{true};
};

} // namespace slog::async

#else

struct Worker
{
};

#endif // SLOG_ASYNC_ENABLED

#endif // SLOG_ASYNC_WORKER_HPP