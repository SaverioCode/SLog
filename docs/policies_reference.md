# MPSCQueue Policies Reference

The `MPSCQueue` (Multiple Producer, Single Consumer) design allows for pluggable policies to handle the "Queue Full" scenario. This decision is critical for the logger's behavior under high load.

## 1. DiscardOnFull (Default)
**Behavior**: If the queue is full, the `push` operation returns `false` immediately, and the item is dropped.
*   **Mechanism**: Atomic check of `head` vs `tail`. No waiting.
*   **Use Case**: Real-time systems where latency is paramount. It is better to lose a log message than to block the application's execution thread.
*   **Pros**: Zero latency impact on the caller.
*   **Cons**: Data loss under bursts.

## 2. BlockOnFull
**Behavior**: If the queue is full, the `push` operation blocks (waits) until space becomes available.
*   **Mechanism**: Uses `std::this_thread::yield()` in a loop (busy-wait with backoff).
*   **Use Case**: Scenarios where data integrity is critical and must not be lost (e.g., audit logs), and where slight backpressure on the application is acceptable.
*   **Pros**: Guaranteed delivery (eventually).
*   **Cons**: Can stall the application thread if the background logger is slow or stuck.

## Design Consideration: Yield vs. Sleep
Currently, `BlockOnFull` uses **Yielding** (`std::this_thread::yield()`) rather than **Sleeping** (Condition Variables).
*   **Yielding**: Keeps the thread in the "runnable" state but gives up the CPU slice.
    *   *Why*: Lower wake-up latency than a full sleep. Checking the atomic flag again is very cheap.
    *   *Trade-off*: potentially higher CPU usage during blockage compared to a deep sleep.
*   **Sleeping**: Removes thread from scheduler completely.
    *   *Why*: Saves CPU.
    *   *Trade-off*: Higher latency to wake up on notify. Requires heavier synchronization primitives (mutex/cond_var) which defeat the purpose of a lock-free-ish queue.

## Recommendation for Users
*   Use `DiscardOnFull` for high-performance, non-critical logging (Trace/Debug).
*   Use `BlockOnFull` for critical systems where logs must persist, accepting that the logger might slow down the main app if disk I/O saturates.
