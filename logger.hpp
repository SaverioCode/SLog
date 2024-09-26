#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include <fstream>
#include <iomanip>

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

#ifndef LOGNAME
#define LOGNAME logger
#endif

namespace sl
{
    class Logger;

    extern Logger& LOGNAME;
}

namespace sl
{
    class NullStream
    {
    public:
        NullStream() = default;
        NullStream(NullStream&) = delete;
        NullStream(NullStream&&) = delete;
        ~NullStream() = default;

        template<typename T>
        NullStream& operator<<(const T&)
        {
            return *this;
        }
    };

    class LoggerStream
    {
    public:
        LoggerStream() = default;
        LoggerStream(std::string& timestamp, const char* level);
        LoggerStream(std::string& timestamp, std::string& level);
        LoggerStream(std::ostream& outstream, std::string& timestamp, const char* level);
        LoggerStream(std::ostream* outstream, std::string& timestamp, const char* level);
        LoggerStream(std::ostream& outstream, std::string& timestamp, std::string& level);
        LoggerStream(LoggerStream&) = delete;
        LoggerStream(LoggerStream&&) = delete;
        ~LoggerStream();

        template<typename T>
        LoggerStream& operator<<(const T& msg)
        {
            _buffer << msg;
            return *this;
        }

        std::string getBuffer() const;

    private:
        std::ostringstream	_buffer;
        std::ostream&		_stream;
    };

	class Logger
	{
	public:
		~Logger();

		static Logger&	getInstance(std::string& file_name, const int options = 0);
		static Logger&	getInstance(const char* file_name = nullptr, const int options = 0);

#ifdef LOGLVLI
		LoggerStream	    info();
#else
		inline NullStream	info();
#endif
#ifdef LOGLVLD
		LoggerStream	    debug();
#else
		inline NullStream	debug();
#endif
#ifdef LOGLVLW
		LoggerStream	    warn();
#else
		inline NullStream	warn();
#endif
#ifdef LOGLVLE
		LoggerStream	    err();
#else
		inline NullStream	err();
#endif
#ifdef LOGLVLF
		LoggerStream	    fatal();
#else
		inline NullStream	fatal();
#endif

		std::string		getFileName() const;

	private:
		Logger() = delete;
		Logger(std::string& file_name, const int options);
		Logger(Logger&) = delete;
		Logger(Logger&&) = delete;

		Logger	operator=(Logger& other) = delete;

		LoggerStream	_print(const char* level);
		LoggerStream	_print(std::ostream* stream, const char* level);
		std::string		_getTimeStamp();
		void			_captureStreams(const int options);

		static std::unique_ptr<Logger>	_instance;
		static std::once_flag			_once_flag;
		const int		_options;
		std::string		_file_name;
		bool			_file_is_set;
		std::ofstream	_outstream;
	};

    /************* LOGGER STREAM *************/

    LoggerStream::LoggerStream(std::string& timestamp, const char* level) : _stream(std::cout)
    {
        _buffer << timestamp << " " << level << " ";
    }

    LoggerStream::LoggerStream(std::string& timestamp, std::string& level) : _stream(std::cout)
    {
        _buffer << timestamp << " " << level << " ";
    }

    LoggerStream::LoggerStream(std::ostream& outstream, std::string& timestamp, const char* level) : _stream(outstream)
    {
        _buffer << timestamp << " " << level << " ";
    }

    LoggerStream::LoggerStream(std::ostream* outstream, std::string& timestamp, const char* level) : _stream(*outstream)
    {
        _buffer << timestamp << " " << level << " ";
    }

    LoggerStream::LoggerStream(std::ostream& outstream, std::string& timestamp, std::string& level) : _stream(outstream)
    {
        _buffer << timestamp << " " << level << " ";
    }

    LoggerStream::~LoggerStream()
    {
        _stream << _buffer.str() << std::endl;
    }

    std::string LoggerStream::getBuffer() const
    {
        return _buffer.str();
    }

    /************* LOGGER *************/

    std::unique_ptr<Logger>	Logger::_instance{ nullptr };
    std::once_flag	Logger::_once_flag{};

    Logger::Logger(std::string& file_name, const int options) : _options(options)
    {
        _file_is_set = false;
        if (file_name.empty() == false) {
            _file_name = file_name;
            _outstream = std::ofstream(file_name);
            if (_outstream.is_open()) {
                _file_is_set = true;
            }
        }
        if (_file_is_set && options != 0) {
            _captureStreams(options);
        }
    }

    Logger::~Logger()
    {
        if (_file_is_set) {
            _outstream.close();
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
    LoggerStream Logger::info()
    {
        return _print("[INFO]");
    }
    #else
    NullStream Logger::info()
    {
        return NullStream();
    }
    #endif

    #ifdef LOGLVLD
    LoggerStream Logger::debug()
    {
        return _print("[DEBUG]");
    }
    #else
    NullStream Logger::debug()
    {
        return NullStream();
    }
    #endif

    #ifdef LOGLVLW
    LoggerStream Logger::warn()
    {
        return _print("[WARNING]");
    }
    #else
    NullStream Logger::warn()
    {
        return NullStream();
    }
    #endif

    #ifdef LOGLVLE
    LoggerStream Logger::err()
    {
        return _print(&std::cerr, "[ERROR]");
    }
    #else
    NullStream Logger::err()
    {
        return NullStream();
    }
    #endif

    #ifdef LOGLVLF
    LoggerStream Logger::fatal()
    {
        return _print(&std::cerr, "[FATAL]");
    }
    #else
    NullStream Logger::fatal()
    {
        return NullStream();
    }
    #endif

    LoggerStream Logger::_print(const char* level)
    {
        std::string time_stamp = "[" + _getTimeStamp() + "]";
        if (_file_is_set) {
            return LoggerStream(_outstream, time_stamp, level);
        }
        else {
            return LoggerStream(time_stamp, level);
        }
    }

    LoggerStream Logger::_print(std::ostream* stream, const char* level)
    {
        std::string time_stamp = "[" + _getTimeStamp() + "]";
        if (_file_is_set) {
            return LoggerStream(_outstream, time_stamp, level);
        }
        else {
            return LoggerStream(stream, time_stamp, level);
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
            std::cout.rdbuf(_outstream.rdbuf());
        }
        if (options & LOGCERR) {
            std::cerr.rdbuf(_outstream.rdbuf());
        }
    }
}
