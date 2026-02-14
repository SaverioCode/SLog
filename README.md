# SLog

[![License](https://img.shields.io/badge/License-MIT-blue.svg?style=flat-square)](LICENSE)
[![Linux](https://img.shields.io/github/actions/workflow/status/SaverioCode/SLog/linux.yml?branch=dev&label=Linux&logo=linux&style=flat-square)](https://github.com/SaverioCode/SLog/actions/workflows/linux.yml)
[![Windows](https://img.shields.io/github/actions/workflow/status/SaverioCode/SLog/windows.yml?branch=dev&label=Windows&logo=windows&logoColor=blue&style=flat-square)](https://github.com/SaverioCode/SLog/actions/workflows/windows.yml)

**Designed for Simplicity. Built for Power**.

- **Singleton**  
- **Thread Safe** (instantiation, prints and log file changes)  
- **One File, Header only**  (less then 1000 lines)
- **5 different log levels**  
- **Fast compilation**  
- **Run-time log file change**
- **Modern C++** (17+)  

![Terminal and File output](assets/terminal_output.png)

## Table of Contents

- [Description](#description)  
- [Requirements](#requirements)  
- [Supported Platform and Compilers](#supported-platforms-and-compilers)  
- [5 Step Guide](#5-step-guide)  
- [Documentation](#documentation)  
- [Contribute](#contribute)  
- [Roadmap](#roadmap)  

## Description

Simple logger is a **singleton**, **thread safe**, **one file** **header only** C++(17+) logger with focus on **simplicity**. It's simple to use and to integrate.  
Less then 5 minutes ([5 Step Guide](#5-step-guide)) to read how integrate it and do it!  
It also use the **RAII** idiom, that means that you don't have to take care about memory managment, but just to use it!

## Requirements

- C++ 20+ (implementation)  
// Todo: - C++ 11+ (interface)

## Supported Platforms and Compilers

### Platforms

- Linux
- Windows  
// Todo: MacOS (specify which?)

### Compilers (implementation)

- gcc (13+)  
// Todo: clang (specify which)
// Todo: mvsc (specify which)

## 3 Step Guide

// Todo: revision it (I think this guide it's not that usefull like this. I could either remove it and opt for an example that show the fastest way to use it inside the code, assuming that the user is already familiar with the basic of C++)

The fastest guide you can have

### 1. Import

Copy the _slogger.hpp_ file from the _include_ folder of this respository and paste it in your project  

### 2. Inclusion

Depending on your project configuration:

- `#include <slog.hpp>` or `#include "slog.hpp"`

### 3. Usage

Usage example:

```c
// Todo: revision it
```

## Wiki

// Todo

### Public Interface

// Todo

```c
```

## Contribute

Suggestion and contribution are welcome! Just contact me and I'll be glad to answer you

## Roadmap

- [x] Implement MPSCQueue tests
- [x] Implement CI/CD, debian, for tests
- [x] Update CI/CD for windows
- [x] Define desing/architecture and Implement Design/architecture professional documentation (piu o meno)
- [x] Define if logger registry will also act as a factory
- [x] Implement LoggerRegistry
- [x] Updatee Logger class (no more need to be singleton, constructor should be visible only to logger registry, it should accept a name, constructor without params not accepted)
- [x] Update Logging Macros to use LoggerRegistry keeping a default case
- [ ] Implement Worker (BackgroundWorker)
- [ ] Integrate Worker in Logger
- [ ] Implement custom LogLevels. Discuss if it's a good idea. and eventually do it.
- [ ] Implement Formatting
- [ ] Implement SAST tests
- [ ] Update CI/CD for SAST tests
- [ ] Implement ConsoleSink
- [ ] Implement optional colors for the different logs levels (console ouotput)
- [ ] Implement Log Rotation
- [ ] Implement fuzzing test
- [ ] Update CI/CD for fuzzing test
- [ ] Improve test suites
- [ ] Implement Benchmark
- [ ] Improve performance
- [ ] Update CI/CD for MacOS ?
- [ ] Write Wiki
- [ ] Implement conan package
- [ ] Implement TCP Sink
- [ ] Implement UDP Sink
- [ ] REENTRANCY
