#pragma once

#include "cpp_rf_signal_toolkit/iq_sample.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <span>
#include <vector>

namespace cpp_rf_signal_toolkit {

enum class RawIqFormat
{
    signed_int16_little_endian,
    signed_int16_big_endian,
    float32_little_endian,
};

class RawIqReader
{
public:
    RawIqReader(const std::filesystem::path& path, RawIqFormat format);

    [[nodiscard]] std::size_t read_samples(std::span<IqSample> output);

private:
    std::filesystem::path path_;
    RawIqFormat format_;
    std::ifstream input_;
    std::vector<char> byte_buffer_;
};

}  // namespace cpp_rf_signal_toolkit
