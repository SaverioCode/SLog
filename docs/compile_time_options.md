# Compile-time Options

Different compile-time options are available to configure the library. Following the C++ philosophy of **"pay for what you use"**

## Table of Content

- [Build](#build)
- [General](#general)
- [Sinks](#sinks)

## Build

It **must** be explicitely defined which kind of library build type is required.

| Macro | Default value | Scope | Notes |
|:------|:-------------:|:------|:------|
|`SLOG_HEADER_ONLY`| `undefined` | Define header only mode | Necessary only if **not** using the project _CMakeLists.txt_  |
|`SLOG_BUILD_TYPE`| `undefined` | Define build type for CMake project library | Values: `STATIC`, `SHARED`, `HEADER_ONLY` |
|`SLOG_BUILD_TESTS`| `OFF` | Include building the tests | Values: `ON`, `OFF`. Only available when using the project _CMakeLists.txt_ |
|`ENABLE_ASAN`| `ON` | Enable AddressSanitizer | Values: `ON`, `OFF`. Only available when using **Tests** and the project _CMakeLists.txt_ |

## General



| Macro | Default value | Scope | Notes |
|:------|:-------------:|:------|:------|
|`SLOG_MAX_LOG_LEVEL`| `6` (`slog::core::LogLevel::TRACE`) | Define Max log level for compile time stripping | |
|`SLOG_ASYNC_ENABLED`| `undefined` | Define async mode | The entire library does **not** use the `<mutex>` header |
|`SLOG_TSAFE`| `undefined` | Define thread safety for sync mode | It includes the `<mutex>` header for **sinks** |
|`SLOG_STREAM_DISABLED`| `undefined` | Define stream logging syntax | It removes `LogProxy`, `NullProxy`, `VodifyLogProxy`, and everything related to streams except for `StreamSink` that have a dedicated macro [check below](#sinks) |

## Sinks

This option are effective **only** when building the library as `STATIC` or `SHARED`.  
Each built-in **sink** is included by default but it can be omitted using the following macros:

| Macro | Default value | Scope | Notes |
|:------|:-------------:|:------|:------|
|`SLOG_STREAM_SINK_DISABLED`| `undefined` | Disable `StreamSink`| |
|`SLOG_FILE_SINK_DISABLED`| `undefined` | Disable `FileSink`| |
