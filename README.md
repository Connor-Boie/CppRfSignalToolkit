# CppRfSignalToolkit

CppRfSignalToolkit is a C++ application for processing and analyzing recorded
radio-frequency (RF) complex in-phase and quadrature (I/Q) samples. The project
is currently in its foundation phase; signal ingestion and digital signal
processing features will be added incrementally.

## Prerequisites

- A C++20 compiler such as GCC 11 or newer
- CMake 3.22 or newer
- Git

GoogleTest is used for automated tests. CMake uses an installed GoogleTest
package when one is available and otherwise downloads the pinned dependency
during configuration.

## Build

Configure and build the project from the repository root:

```sh
cmake -S . -B build
cmake --build build
```

## Run

```sh
./build/src/cpp_rf_signal_toolkit
```

The foundation executable prints the application name. Command-line options
will be introduced alongside the first user-facing features.

## Test

Build the project and run its test suite through CTest:

```sh
cmake --build build
ctest --test-dir build --output-on-failure
```

To configure the application without downloading or building test dependencies,
disable testing explicitly:

```sh
cmake -S . -B build -DBUILD_TESTING=OFF
```
