# Tests

**slogger** tests

## What is tested

- Log levels

## Run tests

1. Create cmake config files

``` bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=../conan/build/Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=release
```

2. Build

``` bash
cmake --build build
```

3. Execute

``` bash
./build/slogger_tests
```
