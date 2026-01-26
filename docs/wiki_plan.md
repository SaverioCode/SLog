# Wiki Structure & Plan

## 1. Homepage (`Home.md`)
- **Hero Section**: "Designed for Simplicity. Built for Power."
- **Quick Links**: 5-minute Guide, API Reference, GitHub.
- **Badges**: CI Status, License, C++11/20.

## 2. User Guides (`guides/`)
- `quick_start.md`: The "Hello World".
- `crash_handling.md`: How to use the CrashGuard.
- `sync_vs_async.md`: When to use which mode (Global switch).
- `sinks_and_formatters.md`: How to change output (File, Console, Rolling).

## 3. Architecture & Design (`design/`)
- `architecture.md`: High-level diagrams, Crash Safety, MMap.
- `queue_design.md`: Deep dive into MPSC/Lock-Free internals.
- `policies.md`: Queue Backpressure strategies (Block vs Drop).
- `benchmarks.md`: Comparison with spdlog, glog, etc.

## 4. Developer Manual (`dev/`)
- `building.md`: How to regenerate the single-header.
- `contributing.md`: Coding style, Branch definitions.
- `testing.md`: Fuzzing, Sanitizers, and Unit Tests.
