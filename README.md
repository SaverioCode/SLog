# Simple Logger  
**As simple as possible**  
- **Singleton**  
- **Thread Safe** (instantiation and prints)  
- **One File, Header only**  (less then 350 lines)
- **5 different log levels**  
- **Fast compilation**  
- **Modern C++**

![Terminal and File output](assets/terminal_output.png)
## Table of Contents
[Description](#description)  
[Contribute](#contribute)  
[Requirements](#requirements)  
[Supported Platform and Compilers](#supported-platforms-and-compilers)  
[5 Step Guide](#5-step-guide)  
[Complete Guide](#complete-guide)  
[Roadmap](#roadmap)  
## Description
Simple logger is a **singleton**, **thread safe**, **one file** **header only** C++(17+) logger with focus on **simplicity**. It's simple to use and to integrate.  
Less then 5 minutes ([5 Step Guide](#5-step-guide)) to read how integrate it and do it!  
It also use the **RAII** idiom, that means that you don't have to take care about memory managment, but just to use it!
## Requirements
- C++ 17+  
## Supported Platforms and Compilers
### Platforms
At this stage I'm not going to provide any officially supported platforms, but for some easy test I excpet it to work on all major linux distros and windows 10+.  
### Compilers
**gnu**, **clang** and unofficially **mvsc**
## 5 Step Guide
The fastest guide you can have
### 1. Import
Copy the _logger.hpp_ file from this repository and paste it in your project
// + bash and batch examples
### 2. Inclusion
- `#include <logger.hpp>` (good practice) be sure that is in your _include_ path  
or  
- `#include "path-to-file/logger.hpp"`    
### 3. Usage
**simple_logger** is a reserved word.  
Usage example:
```c
#include <logger.h>

int main(void)
{
    // Logging in file  (optional line)
    SLOG.init("file.log");

    SLOG.info() << "just use the stream insertion operator as you always do";
}
```
### 5. Compilation
By default all log levels are **disabled** so you need to manually activate them.  
Activate all **log levels** defining macro **LOGLVLA**  
Activate **thread safe prints** defining macro **SLOG_TSAFE**, if you don't need it don't do it  
**gnu**:
```bash
g++ -D LOGLVLA -D SLOG_TSAFE -o exe main.cpp
```
**clang**:
```bash
clang++ -D LOGLVLA -D SLOG_TSAFE S-o exe main.cpp
```
## Documentation
// link to documentation page
## Contribute
Suggestion and contribution are welcome! Just contact me and I'll be glad to answer you
## Roadmap
Not ordered  
- [ ] Implement optional colors for the different logs levels (terminal ouotput)
- [ ] Implement _trace_ log level  
- [ ] Implement test suites
- [ ] Test it with _msvc_ and write examples (it should already work fine)
- [ ] Write the _Complete Guide_
- [ ] Implement CI
- [ ] Test it on different OS (it should already work on major linux distros and windows 10+)   
- [ ] Implement Log Rotation  
- [ ] Implement custom format??
- [ ] Implement verbose??
