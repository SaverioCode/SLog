#ifndef SLOG_ASYNC_POLICIES_HPP
#define SLOG_ASYNC_POLICIES_HPP

#include <slog/async/common.hpp>

namespace slog::async
{

struct DiscardOnFull
{
    template<typename T, typename Queue>
    static bool push(Queue& q, T&& item)
    {
        Reservation r;

        if (q.try_reserve(r)) {
            q.commit(r, std::move(item));
            return true;
        }
        return false;
    }
};

struct BlockOnFull
{
    template<typename T, typename Queue>
    static bool push(Queue& q, T&& item)
    {
        Reservation r = q.reserve();

        q.commit(r, std::move(item));
        return true;
    }
};

} // namespace slog::async

#endif // SLOG_ASYNC_POLICIES_HPP