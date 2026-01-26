#ifndef SLOG_SINKS_STREAM_SINK_HPP
#define SLOG_SINKS_STREAM_SINK_HPP

#include <ostream>

#include <slog/sinks/isink.hpp>

namespace slog::sinks
{

class StreamSink : public ISink
{
    public:
        StreamSink(const std::string& sink_name, std::ostream& ostream) : ISink(sink_name), _ostream(ostream) {}
        ~StreamSink() override
        {
            _ostream.flush();
        }

        SLOG_FORCE_INLINE void    flush() override
        {
            _ostream.flush();
        }

    private:
        void    _write(const LogRecord& record) override
        {
            _ostream << record.message << '\n';
        }

        std::ostream&   _ostream;
};

}

#endif // SLOG_SINKS_STREAM_SINK_HPP
