#ifndef SLOG_SINKS_STREAM_SINK_IPP
#define SLOG_SINKS_STREAM_SINK_IPP

#include <iostream>

namespace slog::sinks
{

// ------------------------
// Public methods
// ------------------------

inline StreamSink::StreamSink(const std::string& sink_name, std::ostream& ostream) 
    : ISink(sink_name), _ostream(ostream) {}

inline StreamSink::~StreamSink()
{
    _ostream.flush();
}

SLOG_FORCE_INLINE void  StreamSink::flush()
{
    _ostream.flush();
}

// ------------------------
// Private methods
// ------------------------

SLOG_FORCE_INLINE void  StreamSink::_write(const slog::core::LogRecord& record)
{
    _ostream << record.message << '\n';
}

}

#endif // SLOG_SINKS_STREAM_SINK_IPP
