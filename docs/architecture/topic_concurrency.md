# Deep Dive: Concurrency & Router Logic (Iteration 7)

## 1. Zero-Cost Mutex Removal (`SLOG_NO_MUTEX`)

**Goal**: When the user defines `SLOG_NO_MUTEX` (or generally in Single-Threaded mode), the compiler should generate **Zero Instructions** for synchronization.

**Mechanism (Macros + Typography)**:
We do *not* use complex template policies for the Mutex itself. We use a type alias.

```cpp
// slog_config.hpp

#if defined(SLOG_NO_MUTEX)
    // The "Null" Mutex
    struct NullMutex {
        void lock() const {}
        void unlock() const {}
        bool try_lock() const { return true; }
    };
    // The Alias
    using SLogMutex = NullMutex;
#else
    #include <mutex>
    using SLogMutex = std::mutex;
#endif

// Usage in SinkManager
// slog_config.hpp

#if defined(SLOG_NO_MUTEX)
    struct NullMutex {
        void lock() const {}
        void unlock() const {}
        bool try_lock() const { return true; }
    };
    // The Alias
    using SLogMutex = NullMutex;
    // Macro to trick std::lock_guard or we define our own ScopeGuard
    #define SLOG_LOCK_GUARD(m) 
#else
    #include <mutex>
    using SLogMutex = std::mutex;
    #define SLOG_LOCK_GUARD(m) std::lock_guard<std::mutex> lock(m);
#endif

// Usage
class SinkManager {
    SLogMutex _mutex; 
    
    void dispatch(const LogRecord& r) {
        SLOG_LOCK_GUARD(_mutex); 
        // ...
    }
};
```

## 2. Compile-Time Router Switching

**User Question**: "At the end of the day the router it's just going to choose between push to the Q or give it directly... do we have a valid reason to do that?"

**Response**:
You are right. The `Router` abstraction might be overkill if it's just `if constexpr (Async) q.push() else sink.log()`.
- **Simplification**: We can remove the `Router` class entirely.
- In `Logger::log()`:
    ```cpp
    void log(const LogRecord& rec) {
        #ifdef SLOG_ASYNC_MODE
            _queue.push(move(rec));
        #else
            _sinkManager.dispatch(rec);
        #endif
    }
    ```
- This satisfies "Keep It Simple". The "Policy" view is useful only if we have *many* async strategies (Overflow, Block, Drop). If we just have one Global Mode, direct `#ifdef` is clearer.
