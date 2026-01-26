#ifndef SLOG_SINKS_FILE_SINK_HPP
#define SLOG_SINKS_FILE_SINK_HPP

#include <cstdio>
#include <exception>

#include <slog/sinks/isink.hpp>

namespace slog::sinks
{

class FileSink : public ISink
{
    public:
        FileSink(const std::string& sink_name, const std::string& file_name, const std::string& mode = "w") : ISink(sink_name)
        {
            _file_name = file_name;

            _fd = std::fopen(file_name.c_str(), mode.c_str());
            if (!_fd) {
                throw std::runtime_error("Failed to open log file.");
            }
        }

        FileSink(const FileSink&) = delete;
        FileSink(FileSink&&) = delete;
        
        ~FileSink() override
        {
            if (_fd) {
                this->flush();
                std::fclose(_fd);
            }
        };

        FileSink& operator=(const FileSink&) = delete;

        SLOG_FORCE_INLINE void  flush() override
        {
            std::fflush(_fd);
        }

        [[nodiscard]] std::string getFileName() const
        {
            return _file_name;
        }

    private:
        void    _write(const LogRecord& record) override
        {
            std::fwrite(record.message.data(), sizeof(char), record.message.size(), _fd);
        }

        std::FILE*  _fd{nullptr};
        std::string _file_name;
};

}

#endif // SLOG_SINKS_FILE_SINK_HPP
