#ifndef SLOG_SINKS_STREAM_SINK_IPP
#define SLOG_SINKS_STREAM_SINK_IPP

#include <iostream>

namespace slog::sinks
{

// ------------------------
// Public methods
// ------------------------

SLOG_INLINE StreamSink::StreamSink(const std::string& sink_name, std::ostream& ostream)
    : ISink(sink_name), _ostream(ostream)
{
}

SLOG_INLINE StreamSink::~StreamSink()
{
    _ostream.flush();
}

SLOG_ALWAYS_INLINE void StreamSink::flush()
{
    _ostream.flush();
}

// ------------------------
// Private methods
// ------------------------

SLOG_ALWAYS_INLINE void StreamSink::_write(const slog::LogRecord& record)
{
    _ostream << record.message << '\n';
}

} // namespace slog::sinks

#endif // SLOG_SINKS_STREAM_SINK_IPP
