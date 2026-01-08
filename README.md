# Simple Logger

**As simple as possible**  

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

Probabli MIT

## Requirements

- C++ 20+  

## Supported Platforms and Compilers

### Platforms

At this stage I'm not going to provide any officially supported platforms, but for some easy test I excpet it to work on all major linux distros and windows 10+.  

### Compilers

**gnu**, **clang** and unofficially **mvsc**

## 5 Step Guide

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
#include <slogger.h>

int main(void)
{
    // Logging in file  (optional line)
    SLOG.init("file.log");

    SLOG.info() << "just use the stream" << "insertion operator as you always do";
}
```

### 5. Compilation

**gnu**:

```bash
g++ -o example main.cpp
```

**clang**:

```bash
clang++ -o example main.cpp
```

## Documentation

// link-to-documentation (work in progress)  

### Public Interface

```c
// Usually you don't actually need them
static Logger&	getInstance(std::string& file_name, const int options = 0);
static Logger&	getInstance(const char* file_name = nullptr, const int options = 0);

// It's possible to eliminate levels from compiled code through macros (No need to change your code)
LoggerStream    info(std::ostream& stream = _cout);
LoggerStream    debug(std::ostream& stream = _cout);
LoggerStream    warn(std::ostream& stream = _cout);
LoggerStream    err(std::ostream& stream = _cerr);
LoggerStream    fatal(std::ostream& stream = _cerr);

bool    updateLogFile(const char* file_name, int options = _options);
bool    updateLogFile(std::string& file_name, int options = _options);
void    updateLogFileOptions(int options);
[[nodiscard]] std::string    getFileName() const;
```

## Contribute

Suggestion and contribution are welcome! Just contact me and I'll be glad to answer you

## Roadmap

- [ ] Implement the possiblity to give a name to the Logger. Like this multiple modules
- [ ] Implement a register class that hold multiple logger, creating a default one
- [ ] Implement optional colors for the different logs levels (console ouotput)
- [ ] Implement test suites
- [ ] Test it with _msvc_ and write examples (it should already work fine)
- [ ] Write Wiki
- [ ] Implement CI
- [ ] Test it on different OS (it should already work on major linux distros and windows 10+)
- [ ] Implement Log Rotation  
- [ ] Implement custom format
