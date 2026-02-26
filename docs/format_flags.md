# Format Flags

The `PatternFormatter` uses a pattern string to define log output format. Flags are prefixed with `%`.

**Default pattern**: `[%Y-%m-%d %H:%M:%S.%e] [%l] %v\n`

## Table of Content

- [Message & Metadata](#message--metadata)
- [Date](#date)
- [Time](#time)
- [Source Location](#source-location)
- [Escaping](#escaping)
- [Custom Flags](#custom-flags)
- [Examples](#examples)

## Message & Metadata

| Flag | Description | Example |
|------|-------------|---------|
| `%v` | Log message | `Hello World` |
| `%l` | Log level (full) | `INFO`, `ERROR` |
| `%L` | Log level (single char) | `I`, `E` |
| `%t` | Thread ID (native OS) | `14320` |
| `%n` | Logger name | `default` |

## Date

| Flag | Description | Example |
|------|-------------|---------|
| `%Y` | Year (4 digits) | `2026` |
| `%m` | Month (01–12) | `02` |
| `%d` | Day (01–31) | `23` |

## Time

| Flag | Description | Example |
|------|-------------|---------|
| `%H` | Hour (00–23) | `12` |
| `%M` | Minute (00–59) | `30` |
| `%S` | Second (00–59) | `45` |
| `%e` | Milliseconds (000–999) | `123` |
| `%f` | Microseconds (000000–999999) | `123456` |

## Source Location

| Flag | Description | Example |
|------|-------------|---------|
| `%s` | Source filename (basename) | `main.cpp` |
| `%g` | Source full path | `/home/user/src/main.cpp` |
| `%#` | Source line number | `42` |
| `%!` | Function name | `main` |

## Escaping

| Sequence | Output |
|----------|--------|
| `%%` | Literal `%` |

Unknown flags (e.g., `%Z`) are treated as literal text.

## Custom Flags

Register custom flags before formatting begins:

```cpp
slog::fmt::PatternFormatter::register_flag('X', my_custom_handler);
```

Handler signature: `void(const slog::fmt::FormatContext& ctx, std::string& dest)`

## Examples

```
Pattern: "[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v\n"
Output:  [2026-02-23 12:30:45.123] [INFO] [14320] Hello World

Pattern: "%H:%M:%S %L/%s:%# %v\n"
Output:  12:30:45 I/main.cpp:42 Hello World
```
