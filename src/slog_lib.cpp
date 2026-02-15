#define SLOG_HEADER_ONLY
#define SLOG_LIB_BUILD

#ifndef SLOG_STREAM_SINK_DISABLED
    #include <slog/sinks/stream_sink.hpp>
#endif

#ifndef SLOG_FILE_SINK_DISABLED
    #include <slog/sinks/file_sink.hpp>
#endif

#include <slog/slog.hpp>
