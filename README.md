# SLog

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
- [Licence](#licence)  
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

## Licence

// Todo: Probably MIT

## Requirements

- C++ 20+ (implementation)  
// Todo: - C++ 11+ (interface)

## Supported Platforms and Compilers

### Platforms

// Todo: Linux (specify which?)  
// Todo: Windows (specify which?)  
// Todo: MacOS (specify which?)

### Compilers

// Todo: gnu (specify which)
// Todo: clang (specify which)
// Todo: mvsc (specify which)

## 5 Step Guide

// Todo: revision it

The fastest guide you can have

### 1. Import

Copy the _slogger.hpp_ file from the _include_ folder of this respository and paste it in your project  

### 2. Inclusion

- `#include <slogger.hpp>` (good practice) be sure that is in your _include_ path  
or  
- `#include "path-to-file/slogger.hpp"`

### 3. Usage

**slogger** is a reserved word.  

Usage example:

```c
// Todo: revision it
```

### 5. Compilation

// Todo: revision it

**gnu**:

```bash
g++ -o example main.cpp
```

**clang**:

```bash
clang++ -o example main.cpp
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
- [ ] Implement CI/CD, debian, for tests
- [ ] Implement Worker (BackgroundWorker)
- [ ] Integrate Worker in Logger
- [ ] Implement naming for Logger
- [ ] Implement LoggerRegistry
- [ ] Update Logging Macros to use LoggerRegistry keeping a default case
- [ ] Implement SinkRegistry
- [ ] Optimize compilation. Separating Definitio from declaration inside the same file. Definition with forward declaration when possible. Wrap the definition with a Macro to easly inject in in one file and use it as a .cpp file for compilation, without effecting other unit compile time
- [ ] Implement Formatting
- [ ] Implement SAST tests
- [ ] Update CI/CD for SAST tests
- [ ] Implement ConsoleSink
- [ ] Implement optional colors for the different logs levels (console ouotput)
- [ ] Implement Log Rotation
- [ ] Implement fuzzing test
- [ ] Update CI/CD for fuzzing test
- [ ] Improve test suites
- [ ] Update CI/CD for windows
- [ ] Implement Benchmark
- [ ] Improve performance
- [ ] Update CI/CD for MacOS ?
- [ ] Write Wiki
- [ ] Implement conan package
- [ ] Implement TCP Sink
- [ ] Implement UDP Sink
- [ ] REENTRANCY
