#ifndef SLOG_ASYNC_MPSC_QUEUE_HPP
#define SLOG_ASYNC_MPSC_QUEUE_HPP

#include <atomic>

#include <slog/common.hpp>
#include <slog/async/common.hpp>

namespace slog::async
{

template<typename T, size_t Size, typename Policy>
class MPSCQueue
{
    static_assert((Size != 0) && ((Size & (Size - 1)) == 0), "MPSCQueue::Size must be a power of 2");

    struct Node
    {
        alignas(64) std::atomic<size_t> seq;
        T                               data;
    };

    public:
        MPSCQueue() : _size(Size), _mask(Size - 1), _buffer(new Node[Size])
        {
            for (size_t i = 0; i < _size; i++) {
                _buffer[i].seq.store(i, std::memory_order_relaxed);
            }
        }
        ~MPSCQueue() { delete[] _buffer; }

        MPSCQueue(const MPSCQueue&) = delete;
        MPSCQueue& operator=(const MPSCQueue&) = delete;

        SLOG_ALWAYS_INLINE bool  push(T&& item)
        {
            return Policy::push(*this, std::move(item));
        }

        bool                    pop(T& item)
        {
            size_t tail = _tail.load(std::memory_order_relaxed);
            Node& node = _buffer[tail & _mask];

            if (node.seq.load(std::memory_order_acquire) != tail + 1) {
                return false;
            }

            item = std::move(node.data);
            node.seq.store(tail + _size, std::memory_order_release);
            // _tail is relaxed, seq's acquire/release enforces visibility
            _tail.store(tail + 1, std::memory_order_relaxed);
            return true;
        }

        [[nodiscard]] bool      tryReserve(Reservation& r)
        {
            size_t head = _head.load(std::memory_order_relaxed);

            while (true) {
                if (head - _tail.load(std::memory_order_acquire) >= _size) {
                    return false;
                }
                if (_head.compare_exchange_weak(head, head + 1, std::memory_order_relaxed)) {
                    r.index = head;
                    return true;
                }
            }
        }

        [[nodiscard]] Reservation reserve()
        {
            Reservation r;
            r.index = _head.fetch_add(1, std::memory_order_relaxed);
            return r;
        }

        void                    commit(const Reservation& r, T&& item)
        {
            Node& node = _buffer[r.index & _mask];

            while (node.seq.load(std::memory_order_acquire) != r.index) {
                std::this_thread::yield();
            }

            node.data = std::move(item);
            node.seq.store(r.index + 1, std::memory_order_release);
        }

    private:
        size_t                          _size;
        size_t                          _mask;
        Node*                           _buffer;
        alignas(64) std::atomic<size_t> _head{0};
        alignas(64) std::atomic<size_t> _tail{0};
};

}

#endif // SLOG_ASYNC_MPSC_QUEUE_HPP
