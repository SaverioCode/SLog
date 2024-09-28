# Simple Logger  
**As simple as possible**  

![Terminal and File output](assets/terminal_output.png)
## Description
Simple logger is a **singleton**, **thread safe**, **header only** C++(17+) logger with focus on **simplicity**. It's simple to use and to integrate.  
You should take no more then 10 minutes ([5 Step Guide](#5-step-guide)) to learn how to use it and integrate it in your own project. And if I'm wrong maybe you are too novice or just dumb! Anyway I like you, so let me know!  
Simple logger use the **RAII** idiom, that means that you don't have to take care about memory managment, but just to use it!
## Contribute
Suggestion and contribution are welcome! Just contact me and I'll be glad to answer you
## Requiriments
- C++ 17+  

It's possible that in future it's going to support older version. At least 14 and maybe 11. Let's see
## Supported Platforms and Compilers
### Platforms
At this stage I'm not going to provide any officially supported platforms, but for some easy test I excpet it to work on all major linux distros and windows 10+.  
### Compilers
**gnu**, **clang** and unofficially **mvsc**
## 5 Step Guide
### 1. Import the _logger.hpp_ file. 
### 2. Include the header where you are going to use it!  
- `#include <logger.hpp>` (good practice) in this case be sure that is in your _include_. If you don't now what I mean for _include_ path check this /* LINK */  
- `#include "path-to-file/logger.hpp"`  
### 3. Declare a _global variable_, maybe in your main.cpp (good practice)  
```c
sl::Logger& SLOG = sl::Logger::getInstance()
```
or
```c
sl::Logger& logger = sl::Logger::getInstance()
```  
**logger** is the default value of **SLOG**! Of course you can use other names for you variable but in this case I suggest you to read the [Complete Guide](#complete-guide)  
If you want to log on a **file**? just pass it as argument the first time you get the instance. After it's not possible.
```c
sl::Logger& SLOG = sl::Logger::getInstance("file_name");
```
### 4. Write a log
```c
SLOG.info() << "just use the stream insertion operator as you always do";
```
### 5. Compile it!
By default all log levels are **disabled** so you need to manually activate them.  
The **simplest** way is to activate them all:  
**gnu**
```bash
g++ -D LOGLVLA -o exe main.cpp -I <path-to-file>
```
**clang**
```bash
clang++ -D LOGLVLA -o exe main.cpp -I <path-to-file>
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
## Roadmap
Not ordered  
- [ ] Implement optional colors for the different logs levels (terminal ouotput)
- [ ] Implement _trace_ log level  
- [ ] Implement test suites
- [ ] Test it with _msvc_ and write examples (it should already work fine)
- [ ] Write the _Complete Guide_
- [ ] Implement CI
- [ ] Test it on different OS (it should already work on major linux distros and windows 10+)   
