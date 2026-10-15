# CppRfSignalToolkit

CppRfSignalToolkit is a C++ application for processing and analyzing recorded
radio-frequency (RF) complex in-phase and quadrature (I/Q) samples. The project
is currently beginning its data-ingestion phase; file readers and digital
signal-processing features will be added incrementally.

## Architecture

Processing code represents an I/Q sample as a single-precision complex value.
The real component stores the in-phase value, and the imaginary component stores
the quadrature value. Future file readers will convert integer and floating-point
recording formats into this common representation so that DSP components do not
depend on a recording's storage encoding.

The initial raw reader supports interleaved signed 16-bit little-endian samples
stored as `I0, Q0, I1, Q1, ...`. Each integer component is divided by 32768 when
it is decoded, producing floating-point values from -1.0 through approximately
0.99997. The reader fills caller-owned buffers so recordings can be processed in
bounded chunks rather than loaded entirely into memory.

```cpp
#include "cpp_rf_signal_toolkit/raw_iq_reader.hpp"

#include <array>

using cpp_rf_signal_toolkit::IqSample;
using cpp_rf_signal_toolkit::RawIqFormat;
using cpp_rf_signal_toolkit::RawIqReader;

RawIqReader reader{"recording.iq", RawIqFormat::signed_int16_little_endian};
std::array<IqSample, 4096> samples;

while (const auto count = reader.read_samples(samples))
{
    // Process samples[0] through samples[count - 1].
}
```

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
