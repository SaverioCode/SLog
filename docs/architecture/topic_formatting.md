# Technical Specification: Formatting & Layout

## 1. Executive Summary
This document defines the formatting architecture for Simple-Logger.
- **V1 Strategy**: **Immediate Interpolation** (Stack Buffer) + **Lock-Free MPSC Queue**.
- **V2 Strategy (Future)**: **Deferred Interpolation** (Byte Buffer) + **Variable-Length Queue**.
- **Layout Responsibility**: Managed by `SinkManager` to enable "Format Once, Write Many" optimization.

---

## 2. Message Interpolation Strategy

### 2.1. Current Approach (V1: Immediate)
The "Payload" (`"Hello {}"`, `42`) is converted to a string on the **Caller Thread**.

*   **Mechanism**:
    1.  `SLOG_INFO` macros use `std::format` (or `fmt`) to render arguments into a `stack_buffer<256>`.
    2.  If the message fits, zero heap allocation occurs.
    3.  The resulting `std::string_view` is stored in the `LogRecord`.
    4.  The `LogRecord` is pushed to the Queue.

*   **Pros**:
    *   **Simplicity**: The Queue stores fixed-size `LogRecord` nodes.
    *   **Safety**: Temporary objects passed as arguments are serialized immediately.
    *   **Cost**: ~100ns per log call (User Thread CPU).

*   **Cons**:
    *   **Throughput**: The formatting cost is paid by the application thread.

### 2.2. Future Approach (V2: Deferred / Zero-Latency)
The "Payload" is effectively `memcpy`'d to the Queue. Formatting happens on the **Worker Thread**.

*   **Mechanism**:
    1.  `SLOG_INFO` serializes arguments (`int: 42`, `float: 3.14`) into a **Raw Byte Buffer**.
    2.  The Queue must be upgraded from `MPSCQueue<LogRecord>` to a **Variable-Length Lock-Free Ring Buffer**.
    3.  The Worker reads the type headers and data, then calls `std::format`.

*   **Implications**:
    *   **Queue Complexity**: High. The Queue must handle wrapping of multi-byte messages.
    *   **Performance**: User Thread cost drops to ~10ns (Memcpy).
    *   **Safety Risk**: Pointers to stack variables are dangerous. (V2 must enforce By-Value copies).

---

## 3. Log Layout Strategy (Context Formatting)

"Layout" refers to the final output format: `[Time] [Level] Message`.

### 3.1. Ownership: SinkManager vs Sinks
**Design Decision**: The `SinkManager` owns the `PatternLayout`.

**Reasoning (The "Format Once" Optimization)**:
*   Most loggers write to multiple sinks (e.g., File + Console).
*   If `PatternLayout` is inside `SinkManager`, we format the timestamp/level/msg **once** into a buffer.
*   We then iterate `sinks` and write that buffer to all of them.
*   *Optimization*: Reduces CPU load by N-1 (where N is number of sinks).

### 3.2. Trade-offs & Cons
**User Question**: "What if each sink needs a different format?" (e.g., JSON for File, Text for Console).

*   **Cons of SinkManager Ownership**:
    *   **Inflexibility**: All sinks *must* accept the same byte buffer. You cannot have one sink output JSON and another output Plain Text under the same Logger *if* the formatting happens upstream.
    *   **Workaround**: If a Sink requires special formatting (e.g., `JsonSink`), it can ignore the pre-formatted buffer and use the raw `LogRecord` (which is also passed to `dispatch`).
    *   **Hybrid Model**:
        1.  `SinkManager` runs the Default Layout -> `string buffer`.
        2.  Passes `(buffer, record)` to `sink->write`.
        3.  `ConsoleSink` uses `buffer` (Fast).
        4.  `JsonSink` ignores `buffer`, reads `record`, and makes JSON (Flexible).

---

## 4. Implementation Details

### 4.1. The PatternLayout Class
```cpp
class PatternLayout {
public:
    // Parses "%d %v" into tokens
    void setPattern(const std::string& pattern);
    
    // Outputs "[12:00] Msg"
    void format(fmt::memory_buffer& out, const LogRecord& rec);
};
```

### 4.2. API & Compatibility
### 3.3. Comparison with State of the Art
**User Question**: "How does Quill handle it? and spdlog?"

*   **SPDLog**:
    *   **Model**: Every Sink has its own Formatter clone.
    *   **Pros**: Infinite flexibility (File gets JSON, Console gets Text).
    *   **Cons**: Performance. If you have 5 sinks, you format the timestamp 5 times.
*   **Quill**:
    *   **Model**: Centralized Formatting. The Backend Thread formats the message *once* (usually), but it supports custom Sinks.
    *   **Cons**: Customizing per-sink format is harder or requires custom sink logic.

**SLog Hybrid Solution Justification**:
We choose **Shared Formatting** (`SinkManager`) as the default because it is faster (O(1) formatting vs O(N) where N=sinks).
The **Hybrid API** (`write(record, buffer)`) allows us to support the "SPDLog Use Case" (Custom JSON Sink) without paying the performance cost for the 99% usage (Text File + Console).
- **Default**: Use `buffer`. (Fast, Shared).
- **Special**: Use `record`. (Flexible, Custom).

