# Requirements Specification

> **Version**: 5.1 (Draft)
> **Status**: In Design (Iteration 6/15)

## 1. Vision
**"Designed for Simplicity. Built for Power."**
The goal is to provide a World-Class C++ Logging Library that outperforms existing solutions while remaining easy to use and integrate.

## 2. Global Configuration & Standards
- **Development Standard**: C++20 (Native implementation).
- **Interface Standard**: C++11 Compatible (User-facing headers)
- **Distribution Model**:
    - **Development**: Multi-file structure (`src/`, `include/`).
    - **Release**: Single Header (`slog.hpp`) generated via script. Uses the **STB-Style** implementation pattern (`#define SLOG_IMPLEMENTATION` to implementation C++20 backend).
- **Execution Modes**: Global Compile-Time Switch.
    - `SLOG_ASYNC_MODE`: High-performance, background thread, non-blocking hot path.
    - `SLOG_SYNC_MODE`: Deterministic, immediate execution, blocking.

## 3. Functional Requirements

### 3.1. Syntax & API
- **Unified Syntax**:
    - Format-based (Primary): `SLOG_INFO("Value: {}", 42);`
    - Stream-based (Legacy/Optional): `SLOG_INFO() << "Value: " << 42;`
- **Formatting Strategy**:
    - **Immediate Formatting**: Arguments are formatted into a stack buffer (e.g., `stack_buffer<256>`) at the call site.
    - **Backend**: Uses `std::format` (C++20) or a high-performance vendor implementation.
    - **Extensibility**: Users can define custom formatters for their types.

### 3.2. Reliability & Safety
- **Crash Safety (CrashGuard)**:
    - **Requirement**: Must attempt to save pending logs upon abrupt termination (`SIGSEGV`, `SIGABRT`).
    - **Mechanism**: A Signal Safe "Manual Drain" that bypasses locks and heap allocations to write directly to a safe file descriptor (e.g., `stderr`).
- **Reentrancy**:
    - **Recursion Guard**: Prevents infinite loops if a Sink triggers a log event (e.g., self-logging errors). Implemented via `thread_local` counters.
- **Thread Safety**:
    - **User Promise**: Custom Sinks do **not** need to handle thread safety. The library guarantees serialized access to sinks (via Mutex in Sync mode, or Single-Threaded Worker in Async mode).

### 3.3. Registry & Lifecycle
- **Factory Pattern**:
    - Loggers and Sinks cannot be created on the stack.
    - Must use `LoggerRegistry::create()` and `SinkRegistry::create()`.
- **Ownership**: The Registries hold ownership of the components to ensure proper lifetime management.

## 4. Non-Functional Requirements
- **Performance**:
    - **Zero-Allocation**: The hot path (logging) should avoid heap allocations for messages fitting in the stack buffer.
    - **Wait-Free Enqueue**: MPSC Ring Buffer.
- **Dependency Free**: STL Only. No external dependencies allowed.
- **Exception Free**: The library functions must use error codes or "no-op" behavior on failure. No `throw`.
- **Configurability**:
    - `SLOG_NO_STREAM`: Strips stream syntax (`<<`) at compile time.
    - `SLOG_NO_MUTEX`: Compile-time option to disable thread-safety primitives in Sync mode (for single-threaded super-fast apps).
- **Quality Assurance**:
    - **Tests**: High coverage (Unit, Integration).
    - **Fuzzing**: Continuous fuzz testing.
    - **Sanitizers**: TSan (Thread), ASan (Address), UBSan (Undefined Behavior).
