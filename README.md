# SLog

[![License](https://img.shields.io/badge/License-MIT-blue.svg?style=flat-square)](LICENSE)
[![Linux](https://img.shields.io/github/actions/workflow/status/SaverioCode/SLog/linux.yml?branch=main&label=Linux&logo=linux&style=flat-square)](https://github.com/SaverioCode/SLog/actions/workflows/linux.yml)
[![Windows](https://img.shields.io/github/actions/workflow/status/SaverioCode/SLog/windows.yml?branch=main&label=Windows&logo=windows&logoColor=blue&style=flat-square)](https://github.com/SaverioCode/SLog/actions/workflows/windows.yml)

**Simple, Reliable, Fast**.

> [!CAUTION]  
> This library is still in development and is not yet considered a beta version.

## Table of Contents

- [Description](#description)  
- [Features](#features)  
- [Requirements](#requirements)  
- [Supported Platform and Compilers](#supported-platforms-and-compilers)  
- [Quick Start](#quick-start)  
- [Documentation](#documentation)  
- [Contribute](#contribute)  
- [Roadmap](#roadmap)  

## Description

The main goal of this library is to provide a **reliable** general purpose logging system for C++ applications, that can be use in most scenarios avoiding to switch between different loggers or changing the code. It's **simple** to start with and to integrate, it's **extensible**, still being very **fast** (~~benchmark~~).

## Features

- **Logger Registry** to ensure loggers/sinks/worker life cycle and address the _Static Destruction Order Fiasco_ without leakages
- **Sync** (default/optional) if _thread safe_ is not enabled, `<mutex>` and `<thread>` headers are not include
- **Async** (optional) lock-free hot path with low latency on user application
- **Unified Interface for Sync and Async**
- **Thread Safe** (optional) _thread safe_ needs to be enabled only for _sync_ mode, for _async_ mode it's just redundant
- **5 different log levels** ~~extesible~~ (FATAL, ERROR, WARNING, INFO, DEBUG) 
- **Compile-Time Log Levels**
- **Runtime Log Levels** (_Registry_ -> _Logger_ -> _Sink_)
- ~~**Fast compilation** C++ 20 modules~~
- **Backpressure Policies** (_async_ mode only) extensible
- **Sinks** extensible (you don't need to worry about thread 
safety implementation)
    - **Console Sink** (built-in)
    - **Stream Sink** (built-in/optional)
    - **File Sink** (built-in/optional)
    - **Custom Sinks**
- **Logging Stream Syntax** (default/optional) it's possible to completely strip out the logging stream syntax
- **Logging Format Strings Syntax** (default) `<format>`
- **Unified Interface for Logging**
- **No external Dependencies** (except for test suites that are not enabled by default)

## Requirements

- C++ 20+

## Supported Platforms and Compilers

### Platforms

- **Linux**
- **Windows**  
- ~~**MacOS** (Todo: test)~~

### Compilers

- **gcc** (13+)  
- **clang** (Todo: check minimun version)  
- **msvc** (Todo: check minimun version)

## Quick Start

> [!NOTE]
> Quick start imply using the library as _header only_ without the project CmakeLists.txt but only `-ISLog/include`, hence compiling with the macro `SLOG_HEADER_ONLY`. In alternative it's possible to include the _CMakeLists.txt_ in your project and use it as a _header only_ or _static/shared_ library leveraging the `SLOG_BUILD_TYPE` option [check compile-time options](docs/compile_time_options.md)

### 1. Installation

Clone the repository

```bash
git clone https://github.com/SaverioCode/SLog.git
```

### 2. Base Usage

```cpp
#include <slog/slog.hpp>

int main()
{
    SLOG_INFO("Hello World {}", 42);
    SLOG_INFO() << "Hello World " << 42;
}
```

Or alternative

```cpp
#include <slog/slog.hpp>

int main()
{
    std::shared_ptr<slog::Logger> logger = slog::Registry::instance()->get_default_logger();

    logger.info("Hello World {}", 42);
    logger.info() << "Hello World " << 42;
}
```

Both code snippets use the **default logger** named "_default_" that comes with a `ConsoleSink` with stream `stdout`.

## Wiki

// Todo

## Contribute

Suggestion and contribution are welcome! Just contact me and I'll be glad to answer you

## Roadmap

It's not ordered and it's more of a Todo list, but Roadmap sounds better.

- [ ] Implement Worker Policies
- [ ] Ensure log level propagation at creation
- [ ] Implement Signal Handler
- [ ] Implement Format Layout
- [ ] Implement Log Rotation
- [ ] Implement Config File
- [ ] Implement optional colors for the different logs levels (console ouotput)
- [ ] Implement SAST tests
- [ ] Implement CI/CD for SAST tests
- [ ] Implement Benchmark
- [ ] Implement fuzzing tests
- [ ] Update tests CI/CD for fuzzing test
- [ ] Improve test suites
- [ ] Improve performance
- [ ] Update CI/CD for MacOS ?
- [ ] Write Wiki
- [ ] Implement conan package
- [ ] Implement custom LogLevels. Discuss if it's a good idea. and eventually do it.
- [ ] Implement TCP Sink
- [ ] Implement UDP Sink
- [ ] Evaluate if implementing Reentrancy guard
