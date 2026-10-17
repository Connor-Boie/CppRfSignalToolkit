#include "cpp_rf_signal_toolkit/raw_iq_reader.hpp"

#include <bit>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>

namespace cpp_rf_signal_toolkit {
namespace {

constexpr std::size_t components_per_sample = 2;
constexpr float int16_scale = 32768.0F;

std::size_t encoded_component_size(RawIqFormat format)
{
    switch (format)
    {
    case RawIqFormat::signed_int16_little_endian:
    case RawIqFormat::signed_int16_big_endian:
        return sizeof(std::int16_t);
    case RawIqFormat::float32_little_endian:
        return sizeof(float);
    }

    throw std::invalid_argument{"Unsupported raw I/Q format"};
}

std::int16_t decode_int16_little_endian(const char* bytes)
{
    const auto low_byte = static_cast<std::uint16_t>(static_cast<unsigned char>(bytes[0]));
    const auto high_byte = static_cast<std::uint16_t>(static_cast<unsigned char>(bytes[1]));
    const auto bits = static_cast<std::uint16_t>(low_byte | (high_byte << 8U));
    return std::bit_cast<std::int16_t>(bits);
}

std::int16_t decode_int16_big_endian(const char* bytes)
{
    const auto high_byte = static_cast<std::uint16_t>(static_cast<unsigned char>(bytes[0]));
    const auto low_byte = static_cast<std::uint16_t>(static_cast<unsigned char>(bytes[1]));
    const auto bits = static_cast<std::uint16_t>((high_byte << 8U) | low_byte);
    return std::bit_cast<std::int16_t>(bits);
}

float decode_float32_little_endian(const char* bytes)
{
    static_assert(sizeof(float) == sizeof(std::uint32_t));
    static_assert(std::numeric_limits<float>::is_iec559);

    const auto byte_0 = static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[0]));
    const auto byte_1 = static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[1]));
    const auto byte_2 = static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[2]));
    const auto byte_3 = static_cast<std::uint32_t>(static_cast<unsigned char>(bytes[3]));
    const auto bits = byte_0 | (byte_1 << 8U) | (byte_2 << 16U) | (byte_3 << 24U);
    return std::bit_cast<float>(bits);
}

float decode_component(const char* bytes, RawIqFormat format)
{
    switch (format)
    {
    case RawIqFormat::signed_int16_little_endian:
        return static_cast<float>(decode_int16_little_endian(bytes)) / int16_scale;
    case RawIqFormat::signed_int16_big_endian:
        return static_cast<float>(decode_int16_big_endian(bytes)) / int16_scale;
    case RawIqFormat::float32_little_endian:
        return decode_float32_little_endian(bytes);
    }

    throw std::invalid_argument{"Unsupported raw I/Q format"};
}

}  // namespace

RawIqReader::RawIqReader(const std::filesystem::path& path, RawIqFormat format)
    : path_{path}, format_{format}, input_{path, std::ios::binary}
{
    static_cast<void>(encoded_component_size(format_));

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

    const auto component_size = encoded_component_size(format_);
    const auto sample_size = component_size * components_per_sample;
    const auto max_stream_bytes = static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max());
    if (output.size() > max_stream_bytes / sample_size)
    {
        throw std::length_error{"Requested I/Q read is too large"};
    }

    const auto requested_bytes = output.size() * sample_size;
    byte_buffer_.resize(requested_bytes);
    input_.read(byte_buffer_.data(), static_cast<std::streamsize>(requested_bytes));

    const auto bytes_read = static_cast<std::size_t>(input_.gcount());
    if (input_.bad() || (input_.fail() && !input_.eof()))
    {
        throw std::runtime_error{"Failed while reading raw I/Q file: " + path_.string()};
    }

    if (bytes_read % sample_size != 0)
    {
        throw std::runtime_error{"Raw I/Q file ends with an incomplete sample: " + path_.string()};
    }

    const auto samples_read = bytes_read / sample_size;
    for (std::size_t sample_index = 0; sample_index < samples_read; ++sample_index)
    {
        const auto byte_index = sample_index * sample_size;
        const auto in_phase = decode_component(byte_buffer_.data() + byte_index, format_);
        const auto quadrature =
            decode_component(byte_buffer_.data() + byte_index + component_size, format_);

        output[sample_index] = {in_phase, quadrature};
    }

    return samples_read;
}

}  // namespace cpp_rf_signal_toolkit
