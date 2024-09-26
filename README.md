# Simple Logger  
**As simple as possible**  
## Description
Simple logger is an header only c++ logger that has no intet to replace complex and feature reach loggers. 
Instead the main focus is on **simplicity**. It's simple to use and to integrate.  
You should take no more then 10 minutes ([5 Step Guide](#5-step-guide)) to learn how to use it and integrate it in your own project. And if I'm wrong maybe you are a novice or just dumb!. Anyway I like you, so let me know!  
Simple logger use the **RAII** idiom, that means that you don't have to take care about memory managment, but just to use it!
## 5 Step Guide
1. #### Import the _logger.hpp_ file. 
2. #### Include the header where you are going to use it!  
    - `#include <logger.hpp>` (good practice) in this case be sure that is in your _include_. If you don't now what I mean for _include_ path check this /* LINK */
    - `#include "path-to-file/logger.hpp"`  
3. #### Declare a **global variable**, maybe in your main.cpp (good practice)  
    ```c++
    sl::Logger& SLOG = sl::Logger::getInstance()
    ```
    or
    ```c++
    sl::Logger& logger = sl::Logger::getInstance()
    ```
    **logger** is the default value of **SLOG**! Of course you can use other names for you variable but in this case I suggest you to read the [Complete Guide](#complete-guide)
4. #### Write a log
    ```c++
    SLOG.info() << "just use the stream insertion operator as you always do";
    ```
5. #### Compile it!
    By default all log levels are **disabled** so you need to manually activate them.  
    The **simplest** way is to activate them all:  
    **gnu**
    ```c++
    g++ -D LOGLVLA -o exec main.cpp -I <path-to-file>
    ```
    **clang**
    ```c++
    clang++ -D LOGLVLA -o exec main.cpp -I <path-to-file>
    ```
## Complete Guide  

## Roadmap
- [ ] Implement optional colors for the different logs levels
- [ ] Implement _trace_ log level  
- [ ] Implement test suites