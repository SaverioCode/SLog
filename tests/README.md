# Tests

**slog** tests

## What is tested

- MPSC queue
- Formatter
- Sinks
    - ConsoleSink
- Async mode

## Run tests

The suggested way it's run from root the _CMakePresets.json_ file.

1. Create cmake config files

``` bash
cmake --preset test
```

2. Build

``` bash
cmake --build --preset test
```

3. Execute

``` bash
ctest --preset test
```
