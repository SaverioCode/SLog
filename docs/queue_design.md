# MPSC Queue: Architectural Specification

This document defines the formal specification, design patterns, and synchronization model for the lock-free Multi-Producer Single-Consumer (MPSC) Circular Buffer.

## 1. Overview
The `MPSCQueue` is a high-performance, non-blocking ring buffer designed for asynchronous log dispatching. It prioritizes low-latency for multiple producer threads while ensuring serialized consumption by a single background worker.

## 2. Structural Design
- **Storage**: Fixed-size array (power of 2) pre-allocated at construction.
- **Addressing**: Uses bitwise masking (`_mask = size - 1`) for wrap-around indexing.
- **Cache Isolation**: `_head` and `_tail` are aligned to separate cache lines (`alignas(64)`) to eliminate False Sharing.

## 3. Synchronization Model: Sequence-Based Ring Buffer
The queue utilizes a **Sequence-per-Node** synchronization pattern for wait-free slot management.

### Slot States and Meaning:
Each slot contains an atomic `seq` sequence number. The state is determined by comparing `seq` with the target `index`:

| Slot State | Meaning | `seq` value |
| :--- | :--- | :--- |
| **Free** | Producer for index `i` may write | `seq == i` |
| **Ready** | Consumer may read | `seq == i + 1` |
| **Consumed** | Slot is free for next wrap (lap) | `seq == i + Size` |

### 4. High-Performance Reservation
The use of sequence numbers provides a robust state handshake, but the method of reserving indices depends on the required behavior:

- **The CAS Loop (Discard-Safe)**: To support policies that discard logs when the queue is full (like `DiscardPolicy`), we must use a **CAS loop in `tryReserve`**.
    - **Fullness Check**: The `head - tail < Size` check must occur *inside* the CAS loop.
    - **Atomicity**: This ensures that we only increment `_head` if there is guaranteed space. If the buffer is full, we return `false` without consuming a sequence number, preventing deadlocks for the consumer.
- **Performance**: While `fetch_add` is slightly faster, the CAS loop provides the necessary correctness for non-blocking "Discard" logic which is critical for logging reliability.

## 5. API Contract: Mechanism + Policy
The queue is designed as a mechanism that delegates control to a **Policy** (Discard, Block, etc.).

### Primitives
- **`Reservation`**: An opaque token containing the reserved slot index.
- **`tryReserve(Reservation&)`**: Uses a CAS loop with integrated capacity checking to safely claim a slot.
- **`commit(Reservation, T&&)`**: Transfers ownership of data to the slot and marks it as ready.
- **`pop(T&)`**: Consumes the next ready item if sequence allows.

## 6. Memory Model
- **Reservations**: `std::memory_order_relaxed` for the head-counter RMW, synchronized via the slot `seq`.
- **Commit**: `std::memory_order_release` when updating the sequence flag to ensure data visibility.
- **Pop**: `std::memory_order_acquire` when loading the sequence flag to synchronize with the producer's data move.
