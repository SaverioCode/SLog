#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <mutex>
#include <memory>
#include <fstream>
#include <iomanip>
#include <optional>

#ifndef SLOG_LVLA
#define SLOG_LVLA 1
#endif

#ifndef SLOG_TSAFE
#define SLOG_TSAFE 1
#endif

#if defined(SLOG_LVLA) && SLOG_LVLA == 1
#define SLOG_LVLI 1
#define SLOG_LVLD 1
#define SLOG_LVLW 1
#define SLOG_LVLE 1
#define SLOG_LVLF 1
#endif

#define SLOG_CERR	0b00000001
#define SLOG_COUT	0b00000010
#define SLOG_COE	0b00000011

#ifndef SLOG
#define SLOG simple_logger
#endif

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

#if defined(SLOG_LVLI) && SLOG_LVLI == 1
    LoggerStream	    info(std::ostream& stream = _cout);
#else
    inline NullStream	info(std::ostream& stream = _cout);
#endif
#if defined(SLOG_LVLD) && SLOG_LVLD == 1
    LoggerStream	    debug(std::ostream& stream = _cout);
#else
    inline NullStream	debug(std::ostream& stream = _cout);
#endif
#if defined(SLOG_LVLW) && SLOG_LVLW == 1
    LoggerStream	    warn(std::ostream& stream = _cout);
#else
    inline NullStream	warn(std::ostream& stream = _cout);
#endif
#if defined(SLOG_LVLE) && SLOG_LVLE == 1
    LoggerStream	    err(std::ostream& stream = _cerr);
#else
    inline NullStream	err(std::ostream& stream = _cerr);
#endif
#if defined(SLOG_LVLF) && SLOG_LVLF == 1
    LoggerStream	    fatal(std::ostream& stream = _cerr);
#else
    inline NullStream	fatal(std::ostream& stream = _cerr);
#endif

bool		updateLogFile(const char* file_name, int options = _options);
bool		updateLogFile(std::string& file_name, int options = _options);
void	    updateLogFileOptions(int options);
[[nodiscard("WHAT IS THE POINT?")]] inline std::string		getFileName() const;

private:
    Logger() = delete;
    Logger(std::string& file_name, const int options);
    Logger(Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger	operator=(Logger& other) = delete;

    LoggerStream	_print(std::ostream& stream = _cout, const char* level = nullptr);
    std::string		_getTimeStamp();
    void			_captureStreams(const int options);

    inline static std::unique_ptr<Logger> _instance{nullptr};
    inline static std::ostream&           _cout{std::cout};
    inline static std::ostream&           _cerr{std::cerr};
    inline static std::once_flag    	  _once_flag{};
    inline static std::mutex	          _log_mutex{};
    inline static int			          _options = 0;
    std::string		                      _file_name;
    std::optional<std::ofstream>    	  _ostream;
};
}

/************* GLOBAL DECLARATION **************/
sl::Logger& SLOG = sl::Logger::getInstance();
extern sl::Logger& SLOG;

namespace sl
{
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

Logger::Logger(std::string& file_name, const int options)
{
	updateLogFile(file_name, options);
}

Logger::~Logger()
{
    if (_ostream.has_value()) {
        _ostream->close();
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
    std::string name = file_name ? file_name : "";
    return Logger::getInstance(name, options);
}

bool	Logger::updateLogFile(const char* file_name, int options)
{
    std::string name = file_name ? file_name : "";
	return updateLogFile(name, options);
}

bool	Logger::updateLogFile(std::string& file_name, int options)
{
#if defined(SLOG_TSAFE) && SLOG_TSAFE == 1
	std::lock_guard<std::mutex> lock(_log_mutex);
#endif
  	bool status = true;
    if (file_name.compare(_file_name) != 0) {
        if (_ostream.has_value()) {
        	_ostream->close();
    	}
    	_file_name = file_name;
  		if (file_name.empty() == false) {
    		_ostream = std::ofstream(file_name);
        }
        else {
            _ostream.reset();
        }
    	if (_ostream.has_value() && _ostream->is_open() == false) {
#if defined(SLOG_TSAFE) && SLOG_TSAFE == 1
			std::cerr << "[" + _getTimeStamp() + "]" << " [ERROR] " << "Failed to open file " << file_name << ": Logging is going to be printed on stdout and stderr";
#else
        	this->err() << "Unable to open file named: " << file_name << ": Logging is going to be printed on stdout and stderr";
#endif
            status = false;
    	}
    }
    updateLogFileOptions(options);
    return status;
}

void	Logger::updateLogFileOptions(int options)
{
	_options = options;
    auto buffer = _options & SLOG_COUT && _ostream.has_value() && _ostream->is_open() ? _ostream->rdbuf() : std::cout.rdbuf();
    _cout.rdbuf(std::move(buffer));
    buffer = _options & SLOG_CERR && _ostream.has_value() && _ostream->is_open() ? _ostream->rdbuf() : std::cerr.rdbuf();
    _cerr.rdbuf(std::move(buffer));
}

#if defined(SLOG_LVLI) && SLOG_LVLI == 1
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

#if defined(SLOG_LVLD) && SLOG_LVLD == 1
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

#if defined(SLOG_LVLW) && SLOG_LVLW == 1
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

#if defined(SLOG_LVLE) && SLOG_LVLE == 1
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

#if defined(SLOG_LVLF) && SLOG_LVLF == 1
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
    std::ostream& outstream = _ostream.has_value() ? _ostream.value() : stream;
    std::mutex* mutex = nullptr;
#ifdef SLOG_TSAFE
    std::lock_guard<std::mutex> lock(_log_mutex);
    mutex = &_log_mutex;
#endif

    return LoggerStream(outstream, time_stamp, level, mutex);
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
    if (options & SLOG_COUT) {
        std::cout.rdbuf(_ostream->rdbuf());
    }
    if (options & SLOG_CERR) {
        std::cerr.rdbuf(_ostream->rdbuf());
    }
}
}
