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
## Complete Guide
### Declaration
There is a macro called `SLOG`. If not manually setted by the user, it's value is `logger`.  
You can still use a different name and the macro usage it's not mandatory.  
The recommended way it's to use `SLOG` or at least the value that has been asigned to it (from you or by default).  
Why? Because the logger it's intendeed to be declared as a _global_variable_ and to simplify the user work has already been made available with the associated macro, exactly like this:
```c
extern Logger& SLOG;
```
This means that you just have to include the header and you are ready to go. Maybe.  
Of course there is one thing to do. **Declare it!**  
The recommended way it's to do it in your _main_ file but do as you please.  
```c
sl::Logger& SLOG = sl::Logger::getInstance();
```
Remember this class is a **singleton**, you don't have access to the constructor and after the first `getInstance()` call, it's done. You are just going to be able to get reference on reference of the same object.  
No init function it's needed just get the instance.
### Log levels
Rigth now there are 5 different log levels, **info**, **debug**, **warning**, **error** and **fatal**. Each log level is associated to a macro in order to efficiently activate and disable what you want without changes in your code.  
`LOGLVLI` for **info**  
`LOGLVLD` for **debug**  
`LOGLVLW` for **warning**  
`LOGLVLE` for **error**  
`LOGLVLF` for **fatal**  
`LOGLVLA` for **all levels**  
As a default behavior **info**, **debug** and **warning** are going to write on `std::cout` instead **error** and **fatal** on `std::cerr`  
### Log on File
Default behavior it's to write on terminal, or at least where the buffer of `std::cout` and `std::cerr` are pointing.  
To write logs on a file you can just pass the _file_name_ as argument to the `Logger::getInstance()` method
```c
sl::Logger& SLOG = sl::Logger::getInstance("<file_name>");
```
Once the file is setted it's not possible to revert it. At least not for now.
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
