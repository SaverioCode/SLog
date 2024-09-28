#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
#include <memory>
#include <fstream>
#include <iomanip>
#include <optional>

#ifdef LOGLVLA
#define LOGLVLI
#define LOGLVLD
#define LOGLVLW
#define LOGLVLE
#define LOGLVLF
#endif

#define LOGCERR	0b00000001
#define LOGCOUT	0b00000010
#define LOGCOE	0b00000011

#ifndef SLOG
#define SLOG logger
#endif

namespace sl
{
    class Logger;

    extern Logger& SLOG;
}

namespace sl
{
class NullStream
{
    public:
        ~NullStream() = default;
        
        template<typename T>
        NullStream& operator<<(const T&)
        {
            return *this;
        }

    private:
        NullStream() = default;
        NullStream(NullStream&) = delete;
        NullStream(NullStream&&) = delete;

        NullStream& operator=(const NullStream&) = delete;

    friend class Logger;
};

class LoggerStream
{
    public:
        ~LoggerStream();

        template<typename T>
        LoggerStream& operator<<(const T& msg)
        {
            _buffer << msg;
            return *this;
        }

        std::string getBuffer() const;

    private:
        LoggerStream() = delete;
        LoggerStream(std::ostream& outstream, std::string& timestamp, const char* level, std::mutex* mutex);

        LoggerStream(LoggerStream&) = delete;
        LoggerStream(LoggerStream&&) = delete;

        LoggerStream& operator=(const LoggerStream&) = delete;

        std::ostringstream    _buffer;
        std::ostream&         _stream;
        std::mutex*           _mutex;

    friend class Logger;
};

class Logger
{
public:
    ~Logger();

    static Logger&	getInstance(std::string& file_name, const int options = 0);
    static Logger&	getInstance(const char* file_name = nullptr, const int options = 0);

#ifdef LOGLVLI
    LoggerStream	    info(std::ostream& stream = _cout);
#else
    inline NullStream	info(std::ostream& stream = _cout);
#endif
#ifdef LOGLVLD
    LoggerStream	    debug(std::ostream& stream = _cout);
#else
    inline NullStream	debug(std::ostream& stream = _cout);
#endif
#ifdef LOGLVLW
    LoggerStream	    warn(std::ostream& stream = _cout);
#else
    inline NullStream	warn(std::ostream& stream = _cout);
#endif
#ifdef LOGLVLE
    LoggerStream	    err(std::ostream& stream = _cerr);
#else
    inline NullStream	err(std::ostream& stream = _cerr);
#endif
#ifdef LOGLVLF
    LoggerStream	    fatal(std::ostream& stream = _cerr);
#else
    inline NullStream	fatal(std::ostream& stream = _cerr);
#endif

    std::string		getFileName() const;

private:
    Logger() = delete;
    Logger(std::string& file_name, const int options);
    Logger(Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger	operator=(Logger& other) = delete;

    LoggerStream	_print(std::ostream& stream = _cout, const char* level = nullptr);
    std::string		_getTimeStamp();
    void			_captureStreams(const int options);

    static std::unique_ptr<Logger>	_instance;
    const int		_options;
    std::string		_file_name;
    std::optional<std::ofstream>	_outstream;
    static std::ostream&   _cout;
    static std::ostream&   _cerr;
    static std::once_flag    _once_flag;
    static std::mutex        _log_mutex;
};

/************* LOGGER STREAM *************/

LoggerStream::LoggerStream(std::ostream& outstream, std::string& timestamp, const char* level, std::mutex* mutex) : _stream(outstream), _mutex(mutex)
{
    _buffer << timestamp << " " << level << " ";
}

LoggerStream::~LoggerStream()
{
#ifdef SLOG_TSAFE
    std::lock_guard<std::mutex> lock(*_mutex);
#endif
    _stream << _buffer.str() << std::endl;
}

std::string LoggerStream::getBuffer() const
{
    return _buffer.str();
}

/************* LOGGER *************/

std::unique_ptr<Logger>	Logger::_instance{ nullptr };
std::once_flag Logger::_once_flag{};
std::mutex Logger::_log_mutex{};
std::ostream& Logger::_cout{std::cout};
std::ostream& Logger::_cerr{std::cerr};

Logger::Logger(std::string& file_name, const int options) : _options(options)
{
    if (file_name.empty() == false) {
        _file_name = file_name;
        _outstream = std::ofstream(file_name);
        if (_outstream.has_value() == false) {
            this->err() << "Unable to open file named: " << file_name << ". Logging is going to be printed on stdout and stderr.";
        }
        if (_outstream.has_value() && options != 0) {
            _captureStreams(options);
        }
    }
}

Logger::~Logger()
{
    if (_outstream.has_value()) {
        _outstream->close();
    }
}

Logger& Logger::getInstance(std::string& file_name, const int options)
{
    struct Temp : public Logger { Temp(std::string& tpm_file_name, const int tmp_options) : Logger(tpm_file_name, tmp_options) {} };
    std::call_once(_once_flag, [&]() { _instance = std::make_unique<Temp>(file_name, options); });
    return *_instance;
}

Logger& Logger::getInstance(const char* file_name, const int options)
{
    std::string name;
    if (file_name) {
        name = std::string(file_name);
    }
    return Logger::getInstance(name, options);
}

#ifdef LOGLVLI
LoggerStream Logger::info(std::ostream& stream)
{
    return _print(stream, "[INFO]");
}
#else
NullStream Logger::info(std::ostream& stream)
{
    return NullStream();
}
#endif

#ifdef LOGLVLD
LoggerStream Logger::debug(std::ostream& stream)
{
    return _print(stream, "[DEBUG]");
}
#else
NullStream Logger::debug(std::ostream& stream)
{
    return NullStream();
}
#endif

#ifdef LOGLVLW
LoggerStream Logger::warn(std::ostream& stream)
{
    return _print(stream, "[WARNING]");
}
#else
NullStream Logger::warn(std::ostream& stream)
{
    return NullStream();
}
#endif

#ifdef LOGLVLE
LoggerStream Logger::err(std::ostream& stream)
{
    return _print(stream, "[ERROR]");
}
#else
NullStream Logger::err(std::ostream& stream)
{
    return NullStream();
}
#endif

#ifdef LOGLVLF
LoggerStream Logger::fatal(std::ostream& stream)
{
    return _print(stream, "[FATAL]");
}
#else
NullStream Logger::fatal(std::ostream& stream)
{
    return NullStream();
}
#endif

LoggerStream Logger::_print(std::ostream& stream, const char* level)
{
    std::string time_stamp = "[" + _getTimeStamp() + "]";
    std::mutex* mutex = nullptr;
#ifdef SLOG_TSAFE
    std::lock_guard<std::mutex> lock(_log_mutex);
    mutex = &_log_mutex;
#endif

    if (_outstream.has_value()) {
        return LoggerStream(_outstream.value(), time_stamp, level, mutex);
    }
    else {
        return LoggerStream(stream, time_stamp, level, mutex);
    }
}

std::string Logger::getFileName() const
{
    return _file_name;
}

std::string Logger::_getTimeStamp()
{
    char formatted[20];

    std::time_t timestamp = std::time(nullptr);
    std::tm* datetime = std::localtime(&timestamp);
    std::strftime(formatted, sizeof(formatted), "%Y-%m-%d %H:%M:%S", datetime);
    return std::string(formatted);
}

void Logger::_captureStreams(const int options)
{
    if (options & LOGCOUT) {
        std::cout.rdbuf(_outstream->rdbuf());
    }
    if (options & LOGCERR) {
        std::cerr.rdbuf(_outstream->rdbuf());
    }
}
}
