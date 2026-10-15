#include "cpp_rf_signal_toolkit/raw_iq_reader.hpp"

#include <bit>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

namespace cpp_rf_signal_toolkit {
namespace {

constexpr std::size_t bytes_per_component = sizeof(std::int16_t);
constexpr std::size_t components_per_sample = 2;
constexpr std::size_t bytes_per_sample = bytes_per_component * components_per_sample;
constexpr float int16_scale = 32768.0F;

std::int16_t decode_int16_little_endian(const char* bytes)
{
    const auto low_byte = static_cast<std::uint16_t>(static_cast<unsigned char>(bytes[0]));
    const auto high_byte = static_cast<std::uint16_t>(static_cast<unsigned char>(bytes[1]));
    const auto bits = static_cast<std::uint16_t>(low_byte | (high_byte << 8U));
    return std::bit_cast<std::int16_t>(bits);
}

}  // namespace

RawIqReader::RawIqReader(const std::filesystem::path& path, RawIqFormat format)
    : path_{path}, input_{path, std::ios::binary}
{
    if (format != RawIqFormat::signed_int16_little_endian)
    {
        throw std::invalid_argument{"Unsupported raw I/Q format"};
    }

    if (!input_.is_open())
    {
        throw std::runtime_error{"Unable to open raw I/Q file: " + path_.string()};
    }
}

std::size_t RawIqReader::read_samples(std::span<IqSample> output)
{
    if (output.empty())
    {
        return 0;
    }

    const auto max_stream_bytes = static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max());
    if (output.size() > max_stream_bytes / bytes_per_sample)
    {
        throw std::length_error{"Requested I/Q read is too large"};
    }

    const auto requested_bytes = output.size() * bytes_per_sample;
    byte_buffer_.resize(requested_bytes);
    input_.read(byte_buffer_.data(), static_cast<std::streamsize>(requested_bytes));

    const auto bytes_read = static_cast<std::size_t>(input_.gcount());
    if (input_.bad() || (input_.fail() && !input_.eof()))
    {
        throw std::runtime_error{"Failed while reading raw I/Q file: " + path_.string()};
    }

    if (bytes_read % bytes_per_sample != 0)
    {
        throw std::runtime_error{"Raw I/Q file ends with an incomplete sample: " + path_.string()};
    }

    const auto samples_read = bytes_read / bytes_per_sample;
    for (std::size_t sample_index = 0; sample_index < samples_read; ++sample_index)
    {
        const auto byte_index = sample_index * bytes_per_sample;
        const auto in_phase = decode_int16_little_endian(byte_buffer_.data() + byte_index);
        const auto quadrature =
            decode_int16_little_endian(byte_buffer_.data() + byte_index + bytes_per_component);

        output[sample_index] = {
            static_cast<float>(in_phase) / int16_scale,
            static_cast<float>(quadrature) / int16_scale,
        };
    }

    return samples_read;
}

}  // namespace cpp_rf_signal_toolkit
