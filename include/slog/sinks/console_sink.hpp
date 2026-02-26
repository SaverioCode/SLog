#ifndef SLOG_SINKS_CONSOLE_SINK_HPP
#define SLOG_SINKS_CONSOLE_SINK_HPP

#include <cstdio>

#include <slog/details/filesystem.hpp>
#include <slog/sinks/isink.hpp>

namespace slog::sinks
{

class ConsoleSink : public ISink
{
public:
    ConsoleSink(const std::string_view name, std::FILE* stream) : ISink(name), _stream(stream) {}

    ConsoleSink(const ConsoleSink&) = delete;
    ConsoleSink(ConsoleSink&&) = delete;
    ~ConsoleSink() override = default;

    ConsoleSink& operator=(const ConsoleSink&) = delete;
    ConsoleSink& operator=(ConsoleSink&&) = delete;

    SLOG_ALWAYS_INLINE void flush() override { std::fflush(_stream); }

private:
    void _write(std::string_view message) override
    {
        if (_stream) [[likely]] {
            slog::details::fwrite_console(message.data(), message.size(), _stream);
        }
    }
    std::FILE* _stream;
};

} // namespace slog::sinks

#endif // SLOG_SINKS_CONSOLE_SINK_HPP
