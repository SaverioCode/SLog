#ifndef SLOG_SINKS_STREAM_SINK_HPP
#define SLOG_SINKS_STREAM_SINK_HPP

#include <iosfwd>

#include <slog/sinks/isink.hpp>

namespace slog::sinks
{

class StreamSink : public ISink
{
    public:
        StreamSink() = delete;
        StreamSink(const StreamSink&) = delete;
        StreamSink(StreamSink&&) = delete;        
        StreamSink(const std::string& sink_name, std::ostream& ostream);
        ~StreamSink() override;

        StreamSink& operator=(const StreamSink&) = delete;
        StreamSink& operator=(StreamSink&&) = delete;

        void    flush() override;

    private:
        void    _write(const slog::core::LogRecord& record) override;

        std::ostream&   _ostream;
};

}

#ifdef SLOG_HEADER_ONLY
    #include <slog/details/sinks/stream_sink.ipp>
#endif // SLOG_HEADER_ONLY

#endif // SLOG_SINKS_STREAM_SINK_HPP
