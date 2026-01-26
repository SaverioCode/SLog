#ifndef SLOG_HPP
#define SLOG_HPP

#include <atomic>
#include <cstdio>
#include <chrono>
#include <exception>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <memory>
#include <sstream>
#include <source_location>
#include <string>
#include <vector>
#include <thread>

#if defined(_MSC_VER)
    #define SLOG_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define SLOG_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define SLOG_FORCE_INLINE inline
#endif

#if defined(SLOG_TSAFE)
    #define SLOG_MUTEX_MEMBER(name) std::mutex name;
    #define SLOG_LOCK(name) std::lock_guard<std::mutex> lock(name);
#else
    #define SLOG_MUTEX_MEMBER(name)
    #define SLOG_LOCK(name)
#endif

#if defined(SLOG_ASYNC_MODE)
    #define SLOG_SINK_MUTEX_MEMBER(name)
    #define SLOG_SINK_LOCK(name)
#else
    #define SLOG_SINK_MUTEX_MEMBER(name) SLOG_MUTEX_MEMBER(name)
    #define SLOG_SINK_LOCK(name)         SLOG_LOCK(name)
#endif

#ifndef SLOG_MAX_LOG_LEVEL
#define SLOG_MAX_LOG_LEVEL 6
#endif

#define SLOG_FATAL SLOG(slog::LogLevel::FATAL)
#define SLOG_ERROR SLOG(slog::LogLevel::ERROR)
#define SLOG_WARN SLOG(slog::LogLevel::WARNING)
#define SLOG_INFO SLOG(slog::LogLevel::INFO)
#define SLOG_DEBUG SLOG(slog::LogLevel::DEBUG)
#define SLOG_TRACE SLOG(slog::LogLevel::TRACE)

#define SLOG(lvl) \
    ((static_cast<uint8_t>(lvl) > SLOG_MAX_LOG_LEVEL) && SLOG_IS_ON(lvl)) ? \
        (void)0 : slog::VodifyLogProxy() & slog::LogProxy(slog::Logger::getInstance(), lvl)

#define SLOG_IS_ON(lvl) \
    (slog::Logger::getInstance().getGlobalLevel() < lvl)

namespace slog
{

enum class LogLevel : uint8_t
{
    FATAL = 1,
    ERROR = 2,
    WARNING = 3,
    INFO = 4,
    DEBUG = 5,
    TRACE = 6
};

constexpr std::string_view to_string(LogLevel level) noexcept
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

struct LogRecord
{
    LogLevel                                level;
    std::string                             message;
    std::source_location                    location;
    std::chrono::system_clock::time_point   timestamp;
    std::thread::id                         thread_id;

    LogRecord() : level(LogLevel::INFO) {}
    LogRecord(LogLevel lvl, std::string&& msg, std::source_location loc) :
        level(lvl), message(std::move(msg)), location(loc),
        timestamp(std::chrono::system_clock::now()), thread_id(std::this_thread::get_id()) {}
    LogRecord(LogRecord&&) noexcept = default;
    LogRecord& operator=(LogRecord&&) noexcept = default;
    LogRecord(const LogRecord&) = default;
    LogRecord& operator=(const LogRecord&) = default;
};

struct Reservation
{
    size_t index;
};

struct DiscardOnFull
{
    template<typename T, typename Queue>
    static bool push(Queue& q, T&& item)
    {
        Reservation r;

        if (q.tryReserve(r)) {
            q.commit(r, std::move(item));
            return true;
        }
        return false;
    }
};

struct BlockOnFull
{
    template<typename T, typename Queue>
    static bool push(Queue& q, T&& item)
    {
        Reservation r = q.reserve();

        q.commit(r, std::move(item));
        return true;
    }
};

template<typename T, size_t Size, typename Policy = DiscardOnFull>
class MPSCQueue
{
    static_assert((Size != 0) && ((Size & (Size - 1)) == 0), "MPSCQueue::Size must be a power of 2");

    struct Node
    {
        alignas(64) std::atomic<size_t> seq;
        T                               data;
    };

    public:
        MPSCQueue() : _size(Size), _mask(Size - 1), _buffer(new Node[Size])
        {
            for (size_t i = 0; i < _size; i++) {
                _buffer[i].seq.store(i, std::memory_order_relaxed);
            }
        }
        ~MPSCQueue() { delete[] _buffer; }

        MPSCQueue(const MPSCQueue&) = delete;
        MPSCQueue& operator=(const MPSCQueue&) = delete;

        SLOG_FORCE_INLINE bool  push(T&& item)
        {
            return Policy::push(*this, std::move(item));
        }
        bool                    pop(T& item)
        {
            size_t tail = _tail.load(std::memory_order_relaxed);
            Node& node = _buffer[tail & _mask];

            if (node.seq.load(std::memory_order_acquire) != tail + 1) {
                return false;
            }

            item = std::move(node.data);
            node.seq.store(tail + _size, std::memory_order_release);
            // _tail is relaxed, seq's acquire/release enforces visibility
            _tail.store(tail + 1, std::memory_order_relaxed);
            return true;
        }
        [[nodiscard]] bool      tryReserve(Reservation& r)
        {
            size_t head = _head.load(std::memory_order_relaxed);

            while (true) {
                if (head - _tail.load(std::memory_order_acquire) >= _size) {
                    return false;
                }
                if (_head.compare_exchange_weak(head, head + 1, std::memory_order_relaxed)) {
                    r.index = head;
                    return true;
                }
            }
        }
        [[nodiscard]] Reservation reserve()
        {
            Reservation r;
            r.index = _head.fetch_add(1, std::memory_order_relaxed);
            return r;
        }
        void                    commit(const Reservation& r, T&& item)
        {
            Node& node = _buffer[r.index & _mask];

            while (node.seq.load(std::memory_order_acquire) != r.index) {
                std::this_thread::yield();
            }

            node.data = std::move(item);
            node.seq.store(r.index + 1, std::memory_order_release);
        }

    private:
        size_t                          _size;
        size_t                          _mask;
        Node*                           _buffer;
        alignas(64) std::atomic<size_t> _head{0};
        alignas(64) std::atomic<size_t> _tail{0};
};

class ISink
{
    public:
        ISink(const std::string& name) : _name(name) {}
        virtual ~ISink() = default;
    
        virtual void    flush() = 0;
        [[nodiscard]] SLOG_FORCE_INLINE LogLevel        getLevel() const noexcept
        {
            return _level;
        }
        [[nodiscard]] SLOG_FORCE_INLINE const std::string& getName() const noexcept
        {
            return _name;
        }
        SLOG_FORCE_INLINE void    log(const LogRecord& record)
        {
            SLOG_SINK_LOCK(_sink_mutex)
            _write(record);
        }
        SLOG_FORCE_INLINE void            setLevel(const LogLevel level) noexcept
        {
            _level = level;
        }
    
    protected:
        virtual void    _write(const LogRecord& record) = 0;

    private:
        std::string     _name;
        LogLevel        _level{LogLevel::TRACE};
        SLOG_SINK_MUTEX_MEMBER(_sink_mutex)
};

class StreamSink : public ISink
{
    public:
        StreamSink(const std::string& sink_name, std::ostream& ostream) : ISink(sink_name), _ostream(ostream)
        {
        }
        ~StreamSink() override
        {
            _ostream.flush();
        }

        SLOG_FORCE_INLINE void    flush() override
        {
            _ostream.flush();
        }

    private:
        void    _write(const LogRecord& record) override
        {
            _ostream << record.message << '\n';
        }

        std::ostream&   _ostream;
};

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

        SLOG_FORCE_INLINE void        flush() override
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

class SinkManager
{
    using SinkVec = std::vector<std::shared_ptr<ISink>>;
    using SinkPtr = std::shared_ptr<SinkVec>;
    
    public:
        SinkManager() : _sinks_ptr(std::make_shared<SinkVec>()) {}
        
        bool                    addSink(std::shared_ptr<ISink> sink)
        {
            SLOG_LOCK(_sink_manager_mutex)
            auto name = sink->getName();
            auto current_sinks = _sinks_ptr.load(std::memory_order_relaxed);
            SinkPtr new_sinks;

            for (const auto& existing_sink : *current_sinks) {
                if (existing_sink->getName() == name) {
                    return false;
                }
            }
            new_sinks = std::make_shared<SinkVec>(*current_sinks);
            new_sinks->push_back(sink);
            _sinks_ptr.store(new_sinks, std::memory_order_release);
            return true;
        }
        void                    dispatch(const LogRecord& record)
        {
            auto current_sinks = _sinks_ptr.load(std::memory_order_acquire);

            for (auto& sink : *current_sinks) {
                if (record.level <= sink->getLevel()) {
                    sink->log(record);
                }
            }
        }
        [[nodiscard]] std::shared_ptr<ISink>  getSink(const std::string& name)
        {
            auto current_sinks = _sinks_ptr.load(std::memory_order_acquire);

            for (const auto& sink : *current_sinks) {
                if (sink->getName() == name) {
                    return sink;
                }
            }
            return nullptr;
        }
        [[nodiscard]] SLOG_FORCE_INLINE SinkPtr getSinkList() const noexcept
        {
            return _sinks_ptr.load(std::memory_order_acquire);
        }
        void                    removeSink(const std::string& name)
        {
            SLOG_LOCK(_sink_manager_mutex)
            auto current_sinks = _sinks_ptr.load(std::memory_order_relaxed);

            for (auto it = current_sinks->begin(); it != current_sinks->end(); it++) {
                if ((*it)->getName() == name) {
                    auto new_sinks = std::make_shared<SinkVec>(*current_sinks);

                    new_sinks->erase(std::next(new_sinks->begin(), std::distance(current_sinks->begin(), it)));
                    _sinks_ptr.store(new_sinks, std::memory_order_release);
                    return;
                }
            }
        }

    private:
        std::atomic<SinkPtr> _sinks_ptr;
        SLOG_MUTEX_MEMBER(_sink_manager_mutex)
};

struct NullProxy
{
    template<typename T>
    constexpr NullProxy& operator<<(const T&) noexcept { return *this; }

    template<typename... Args>
    constexpr void operator()(std::string_view, Args&&...) noexcept {}
};

class Logger;
class LogProxy
{
public:
    SLOG_FORCE_INLINE LogProxy(Logger& logger, LogLevel level,
        bool is_active = true, std::source_location loc = std::source_location::current()) :
        _logger(logger), _level(level), _is_active(is_active), _location(loc) {}

    LogProxy(const LogProxy&) = delete;
    LogProxy(LogProxy&&) = delete;
    ~LogProxy(); // Defined below due to circular dependency with Logger class

    LogProxy& operator=(const LogProxy&) = delete;
    LogProxy& operator=(LogProxy&&) = delete;

    template<typename T>
    LogProxy& operator<<(const T& msg) { return _stream_write(msg); }
    LogProxy& operator<<(std::ostream& (*manip)(std::ostream&)) { return _stream_write(manip); }
    LogProxy& operator<<(std::ios_base& (*manip)(std::ios_base&)) { return _stream_write(manip); }

    template<typename... Args>
    LogProxy& operator()(std::format_string<Args...> fmt, Args&&... args)
    {
        std::format_to(std::back_inserter(_string_buffer), fmt, std::forward<Args>(args)...);
        return *this;
    }

private:
    friend class Logger;

    SLOG_FORCE_INLINE void _ensure_stream()
    {
        if (!_stream_buffer) [[unlikely]] {
            _stream_buffer = std::make_unique<std::ostringstream>();
        }
    }
    template<typename V>
    LogProxy& _stream_write(V&& val)
    {
        _ensure_stream();
        (*_stream_buffer) << std::forward<V>(val);
        return *this;
    }

    Logger&                             _logger;
    LogLevel                            _level;
    bool                                _is_active;
    std::source_location                _location;
    std::string                         _string_buffer; 
    std::unique_ptr<std::ostringstream> _stream_buffer; 
};

struct VodifyLogProxy
{
    void operator&(const LogProxy&) {}
};

class Logger
{
public:
    ~Logger() = default;

    SLOG_FORCE_INLINE static Logger&	getInstance(const int options = 0)
    {
        struct Temp : public Logger { Temp(const int tmp_options) : Logger(tmp_options) {} };
        std::call_once(_once_flag, [&]() { _instance = std::make_unique<Temp>(options); });
        return *_instance;
    }

    template<LogLevel level>
    SLOG_FORCE_INLINE auto log()
    {
        if constexpr (static_cast<uint8_t>(level) > SLOG_MAX_LOG_LEVEL) {
            return NullProxy{};
        }
        return LogProxy(*this, level, level <= _global_log_level); 
    }
    template<LogLevel level, typename... Args>
    void log(std::format_string<Args...> fmt, Args&&... args)
    {
        std::string string_buffer;

        if constexpr (static_cast<uint8_t>(level) > SLOG_MAX_LOG_LEVEL) {
            return;
        }
        std::format_to(std::back_inserter(string_buffer), fmt, std::forward<Args>(args)...);
        _submit(level, std::move(string_buffer), std::source_location::current());
    }

    SLOG_FORCE_INLINE auto fatal() { return log<LogLevel::FATAL>(); }
    template<typename... Args>
    SLOG_FORCE_INLINE auto fatal(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::FATAL>(fmt, std::forward<Args>(args)...);
    }
    SLOG_FORCE_INLINE auto error() { return log<LogLevel::ERROR>(); }
    template<typename... Args>
    SLOG_FORCE_INLINE auto error(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::ERROR>(fmt, std::forward<Args>(args)...);
    }
    SLOG_FORCE_INLINE auto warn()  { return log<LogLevel::WARNING>(); }
    template<typename... Args>
    SLOG_FORCE_INLINE auto warn(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::WARNING>(fmt, std::forward<Args>(args)...);
    }
    SLOG_FORCE_INLINE auto info()  { return log<LogLevel::INFO>(); }
    template<typename... Args>
    SLOG_FORCE_INLINE auto info(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::INFO>(fmt, std::forward<Args>(args)...);
    }
    SLOG_FORCE_INLINE auto debug() { return log<LogLevel::DEBUG>(); }
    template<typename... Args>
    SLOG_FORCE_INLINE auto debug(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::DEBUG>(fmt, std::forward<Args>(args)...);
    }
    SLOG_FORCE_INLINE auto trace() { return log<LogLevel::TRACE>(); }
    template<typename... Args>
    SLOG_FORCE_INLINE auto trace(std::format_string<Args...> fmt, Args&&... args)
    {
        return log<LogLevel::TRACE>(fmt, std::forward<Args>(args)...);
    }

    SLOG_FORCE_INLINE void    addSink(std::shared_ptr<ISink> sink)
    {
        _sink_manager.addSink(sink);
    }
    SLOG_FORCE_INLINE void    removeSink(const std::string& name) noexcept
    {
        _sink_manager.removeSink(name);
    }

    [[nodiscard]] SLOG_FORCE_INLINE LogLevel    getGlobalLevel() const noexcept
    {
        return _global_log_level;
    }
    SLOG_FORCE_INLINE void                      setGlobalLevel(const LogLevel level) noexcept
    {
        _global_log_level = level;
    }

private:
    friend struct LogProxy;

    Logger() = delete;
    Logger(const int options) {} // Todo: option for having an outof the box std::cout sink?
    Logger(Logger&) = delete;
    Logger(Logger&&) = delete;

    Logger	operator=(Logger& other) = delete;
    Logger  operator=(Logger&& other) = delete;


    void            _submit(const LogLevel level, std::string&& message, std::source_location loc)
    {
        LogRecord record{level, std::move(message), loc};

        _sink_manager.dispatch(record);
    }
    [[nodiscard]] std::string		_getTimeStamp()
    {
        char formatted[20];

        std::time_t timestamp = std::time(nullptr);
        std::tm* datetime = std::localtime(&timestamp);
        std::strftime(formatted, sizeof(formatted), "%Y-%m-%d %H:%M:%S", datetime);
        return std::string(formatted);
    }

    inline static std::unique_ptr<Logger>   _instance{nullptr};
    inline static SinkManager               _sink_manager{};
    inline static std::once_flag    	    _once_flag{};
    inline static std::mutex	            _log_mutex{};
    LogLevel                                _global_log_level{LogLevel::TRACE};
};
}

inline slog::LogProxy::~LogProxy()
{
    // Log level behaviour safeguard when logging is done calling methods instead of using MACROS
    if (!_is_active) [[unlikely]] {
        return;
    }
    // Format strings are preferred for performance
    if (_stream_buffer) [[unlikely]] {
        // The else is to prevent unintended usage from user
        // who might use both streams and format strings for one log line
        if (_string_buffer.empty()) [[likely]] {
            _string_buffer = std::move(*_stream_buffer).str();
        }
        else {
            _string_buffer.append(std::move(*_stream_buffer).str());
        }
    }
    _logger._submit(_level, std::move(_string_buffer), _location);
}

#endif // SLOGGER_HPP
