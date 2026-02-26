#ifndef SLOG_SINKS_FILE_SINK_HPP
#define SLOG_SINKS_FILE_SINK_HPP

#include <exception>
#include <iosfwd>

#include <slog/details/filesystem.hpp>
#include <slog/sinks/isink.hpp>

namespace slog::sinks
{

class FileSink : public ISink
{
public:
    FileSink(const std::string_view sink_name, const std::string_view file_name,
             const std::string_view mode = "w")
        : ISink(sink_name)
    {
        _file_name = file_name;

        _stream = slog::details::fopen(file_name.data(), mode.data());
        if (!_stream) {
            throw std::runtime_error("Failed to open log file.");
        }
    }

    FileSink(const FileSink&) = delete;
    FileSink(FileSink&&) = delete;

    ~FileSink() override
    {
        if (_stream) {
            this->flush();
            std::fclose(_stream);
        }
    };

    FileSink& operator=(const FileSink&) = delete;

    SLOG_ALWAYS_INLINE void flush() override { std::fflush(_stream); }

    [[nodiscard]] std::string get_file_name() const { return _file_name; }

private:
    void _write(std::string_view message) override
    {
        slog::details::fwrite_file(message.data(), message.size(), _stream);
    }

    std::FILE* _stream{nullptr};
    std::string _file_name;
};

} // namespace slog::sinks

#endif // SLOG_SINKS_FILE_SINK_HPP
