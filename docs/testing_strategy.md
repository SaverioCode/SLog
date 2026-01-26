# Testing Strategy

## Framework
We use **Google Test (GTest)** for unit testing.
- **Reason**: Industry standard, powerful features (Typed Tests, Death Tests), and easy integration via CMake/Conan.
- **Philosophy**: Dev-dependencies are acceptable as long as the core library remains header-only and dependency-free.

## Organization
Tests are located in `tests/src/` and organized by component to ensure isolation:
- `test_main.cpp`: Entry point for the test runner.
- `mpsc_queue/`:
    - `single_thread_tests.cpp`: Functional and logic tests for `MPSCQueue`.
    - `multi_thread_tests.cpp`: Concurrent integrity tests.
- `test_logger.cpp`: General logger logic, levels, and macros.
- `test_sinks.cpp`: Tests for specific sinks (`StreamSink`, `FileSink`).

## MPSCQueue Test Plan

Since `MPSCQueue` is a core concurrent component, it requires rigorous verification.

### 1. Functional / Sequential Tests (Single Thread)
*   **Basic Operations**: Verify `push` and `pop` mechanics with simple types (e.g., `int`).
*   **FIFO Ordering**: Ensure elements are retrieved in the exact order they were inserted.
*   **Wrap Around Logic**: Validate ring buffer index arithmetic by filling and emptying the queue multiple times.
*   **Capacity & Policy**:
    *   Fill the queue to its maximum size.
    *   **DiscardOnFull**: Verify that pushing to a full queue returns `false` and drops the item.
    *   **BlockOnFull**: (To be tested in multi-threaded context primarily).

### 2. Concurrency / Integrity Tests (Multi-Threaded)
*   **Producer-Consumer**:
    *   Spawn $K$ producer threads.
    *   Each thread pushes $N$ items.
    *   Main thread consumes.
    *   **Verification**: Ensure total items popped equals $K \times N$.
*   **DiscardOnFull Stress**:
    *   Small queue, many producers to force drops.
    *   **Verification**: Ensure strict monotonicity of received sequences (gaps allowed, reordering forbidden). Ensure total items < total produced testing the drop mechanic.
*   **Data Integrity**:
    *   Producers push monotonic sequences.
    *   Consumer verifies that for each thread ID, the sequence of received numbers is strictly increasing (no data corruption or reordering within a single producer's stream).

### 3. Stress Tests (NOT YET IMPLEMENTED)
*   **High Contention**: High-frequency push operations from many threads to stress-test atomic variables and catch race conditions.
*   **Edge Cases**: Rapid start/stop of threads, queue destruction while threads are active (if applicable).

## Tools & Sanitizers
*   **AddressSanitizer (ASan)**: Enabled by default for tests to catch memory errors (buffer overflows, use-after-free).
*   **ThreadSanitizer (TSan)**: Recommended for occasional integrity checks of `MPSCQueue`.
    *   *Note*: Run separately from ASan. Can produce false positives with some lock-free patterns/yields; verify manually.

