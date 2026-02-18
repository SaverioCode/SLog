#ifndef SLOG_ASYNC_ASYNC_OP_HPP
#define SLOG_ASYNC_ASYNC_OP_HPP

#include <slog/core/log_record.hpp>
#include <slog/sinks/sink_manager.hpp>

namespace slog::async
{
struct AsyncOp
{
    slog::LogRecord record;
    slog::sinks::SinkManager* sink_manager;
};
} // namespace slog::async

#endif // SLOG_ASYNC_ASYNC_OP_HPP
