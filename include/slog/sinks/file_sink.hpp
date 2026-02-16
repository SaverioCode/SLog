#ifndef SLOG_SINKS_FILE_SINK_HPP
#define SLOG_SINKS_FILE_SINK_HPP

#include <cstdio>
#include <exception>
#include <iosfwd>

#include <slog/sinks/isink.hpp>

namespace slog::sinks
{

class FileSink : public ISink
{
    public:
        FileSink(const std::string& sink_name, const std::string& file_name, const std::string& mode = "w") : ISink(sink_name)
        {
            _file_name = file_name;

            _stream = std::fopen(file_name.c_str(), mode.c_str());
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

        SLOG_ALWAYS_INLINE void  flush() override
        {
            std::fflush(_stream);
        }

        [[nodiscard]] std::string get_file_name() const
        {
            return _file_name;
        }

    private:
        void    _write(const slog::LogRecord& record) override
        {
            slog::sinks::fwrite_file(record.message.data(), record.message.size(), _stream);
        }

        std::FILE*  _stream{nullptr};
        std::string _file_name;
};

}

#endif // SLOG_SINKS_FILE_SINK_HPP
