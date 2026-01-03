#ifndef SLOGGER_HPP
#define SLOGGER_HPP

#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

// Todo: remove? or not remove?
#ifndef SLOG_TSAFE
#define SLOG_TSAFE
#endif

#if defined(SLOG_LVLT)
#define SLOG_LVLD
#endif
#if defined(SLOG_LVLD)
#define SLOG_LVLI
#endif
#if defined(SLOG_LVLI)
#define SLOG_LVLW
#endif
#if defined(SLOG_LVLW)
#define SLOG_LVLE
#endif
#if defined(SLOG_LVLE)
#define SLOG_LVLF
#endif

#define SLOG_CERR	0b00000001
#define SLOG_COUT	0b00000010
#define SLOG_COE	0b00000011

#ifndef SLOG
#define SLOG slogger
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

        std::ostringstream    _buffer{};
        std::ostream&         _stream;
        std::mutex*           _mutex;

    friend class Logger;
};

enum class LogLevel : uint8_t
{
    FATAL = 1,
    ERROR = 2,
    WARNING = 3,
    INFO = 4,
    DEBUG = 5,
    TRACE = 6
};

class Logger
{
public:
    ~Logger();

    static Logger&	getInstance(std::string& file_name, const int options = 0);
    static Logger&	getInstance(const char* file_name = nullptr, const int options = 0);
    
#if defined(SLOG_LVLF)
    LoggerStream	    fatal(std::ostream& stream = _cerr);
#else
    inline NullStream	fatal(std::ostream& stream = _cerr);
#endif
#if defined(SLOG_LVLE)
    LoggerStream	    err(std::ostream& stream = _cerr);
#else
    inline NullStream	err(std::ostream& stream = _cerr);
#endif
#if defined(SLOG_LVLW)
    LoggerStream	    warn(std::ostream& stream = _cout);
#else
    inline NullStream	warn(std::ostream& stream = _cout);
#endif
#if defined(SLOG_LVLI)
    LoggerStream	    info(std::ostream& stream = _cout);
#else
    inline NullStream	info(std::ostream& stream = _cout);
#endif
#if defined(SLOG_LVLD)
    LoggerStream	    debug(std::ostream& stream = _cout);
#else
    inline NullStream	debug(std::ostream& stream = _cout);
#endif
#if defined(SLOG_LVLT)
    LoggerStream	    trace(std::ostream& stream = _cout);
#else
    inline NullStream	trace(std::ostream& stream = _cout);
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
/************* HELPER FUNCTIONS *************/

constexpr std::string_view to_string(LogLevel level)
{
    switch (level) {
        case LogLevel::FATAL:  return "FATAL";
        case LogLevel::ERROR:  return "ERROR";
        case LogLevel::WARNING:return "WARNING";
        case LogLevel::INFO:   return "INFO";
        case LogLevel::DEBUG:  return "DEBUG";
        case LogLevel::TRACE:  return "TRACE";
        default:               return "UNKNOWN";
    }
}

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
#if defined(SLOG_TSAFE)
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
#if defined(SLOG_TSAFE)
            std::cerr << "[" + _getTimeStamp() + "]" << " [ERROR] " << "Failed to open file " << file_name << ": Logging is going to be printed on console only";
#else
            this->err() << "Unable to open file named: " << file_name << ": Logging is going to be printed on console only";
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

#if defined(SLOG_LVLF)
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

#if defined(SLOG_LVLE)
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

#if defined(SLOG_LVLW)
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

#if defined(SLOG_LVLI)
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

#if defined(SLOG_LVLD)
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

#if defined(SLOG_LVLT)
LoggerStream Logger::trace(std::ostream& stream)
{
    return _print(stream, "[TRACE]");
}
#else
NullStream Logger::trace(std::ostream& stream)
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

#endif // SLOGGER_HPP
