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

The main goal of this library is to provide a **reliable** general purpose logging system for C++ applications, that can be use in most scenarios avoiding to switch between different loggers or changing the code. It's **simple** to start with and to integrate, it's **extensible**, still being very **fast** (~~benchmark~~). If you are looking for truly high performance logging or you need to work on bare metal (no allocations), you should look for other libraries.

## Features

- **Logger's Registry** (default/optional) to ensure loggers/synks life cycle and address the _Static Destruction Order Fiasco_ without leakages
- **Sync** (default/optional)
- **Async** (optional) completely lock-free with low latency on user application
- **Unified Interface for Sync and Async**
- **Thread Safe** (optional) _async_ mode it's thread safe by design, for _sync_ mode it must be enabled to protect _synks_
- **No Mutex** (default/optional) _async_ mode never includes `<mutex>`, for _sync_ mode _thread safe_ must not be enabled
- **5 different log levels** ~~extesible~~ (FATAL, ERROR, WARNING, INFO, DEBUG) 
- **Compile-Time Log Levels**
- **Runtime Log Levels** (_Registry_ -> _Logger_ -> _Sink_)
- ~~**Fast compilation** C++ 20 modules~~
- **Backpressure Policies** (_async_ mode only) extensible
- **Sinks** extensible (you don't need to worry about thread 
safety)
    - ~~**Console Sink** (built-in/optional)~~
    - **Stream Sink** (built-in/optional)
    - **File Sink** (built-in/optional)
    - **Custom Sinks**
- **Logging Stream Syntax** (default/optional) it's possible to completely strip out the logging stream syntax
- **Logging Format Strings Syntax** (default) `<format>`
- **Unified Interface for Logging**
- **No external Dependencies** (except for the test suite)

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
> Quick start imply using it as a header only library without using cmake but only `-ISLog/include`, hence compiling with the macro `SLOG_HEADER_ONLY`.

### 1. Installation

Clone the repository

```bash
git clone https://github.com/SaverioCode/SLog.git
```

### 2. Base Usage

```cpp
#include <slog.hpp>

int main()
{
    SLOG_INFO("Hello World {}", 42);
    SLOG_INFO() << "Hello World " << 42;
}
```

Or alternative

```cpp
#include <slog.hpp>

int main()
{
    slog::Logger logger = slog::Registry::instance()->get_default_logger();

    logger.info("Hello World {}", 42);
    logger.info() << "Hello World " << 42;
}
```

Both code snippets use the **default logger** named "_default_".

## Wiki

// Todo

## Contribute

Suggestion and contribution are welcome! Just contact me and I'll be glad to answer you

## Roadmap

- [ ] Implement Worker (BackgroundWorker)
- [ ] Integrate Worker in Logger
- [ ] Implement Formatting
- [ ] Implement Log Rotation
- [ ] Implement ConsoleSink
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
