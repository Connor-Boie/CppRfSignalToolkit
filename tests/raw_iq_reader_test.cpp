#include "cpp_rf_signal_toolkit/raw_iq_reader.hpp"

#include <gtest/gtest.h>

#include <array>
#include <bit>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <span>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

namespace cpp_rf_signal_toolkit {
namespace {

void append_int16_little_endian(std::vector<unsigned char>& bytes, std::int16_t value)
{
    const auto bits = std::bit_cast<std::uint16_t>(value);
    bytes.push_back(static_cast<unsigned char>(bits & 0x00FFU));
    bytes.push_back(static_cast<unsigned char>((bits >> 8U) & 0x00FFU));
}

void append_int16_big_endian(std::vector<unsigned char>& bytes, std::int16_t value)
{
    const auto bits = std::bit_cast<std::uint16_t>(value);
    bytes.push_back(static_cast<unsigned char>((bits >> 8U) & 0x00FFU));
    bytes.push_back(static_cast<unsigned char>(bits & 0x00FFU));
}

void append_sample(std::vector<unsigned char>& bytes, std::int16_t in_phase, std::int16_t quadrature)
{
    append_int16_little_endian(bytes, in_phase);
    append_int16_little_endian(bytes, quadrature);
}

void append_big_endian_sample(
    std::vector<unsigned char>& bytes,
    std::int16_t in_phase,
    std::int16_t quadrature)
{
    append_int16_big_endian(bytes, in_phase);
    append_int16_big_endian(bytes, quadrature);
}

class TemporaryBinaryFile
{
public:
    TemporaryBinaryFile(const std::string& filename, const std::vector<unsigned char>& bytes)
        : path_{std::filesystem::temp_directory_path() / filename}
    {
        std::ofstream output{path_, std::ios::binary | std::ios::trunc};
        if (!output)
        {
            throw std::runtime_error{"Unable to create temporary test file"};
        }

        output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
        if (!output)
        {
            throw std::runtime_error{"Unable to write temporary test file"};
        }
    }

    ~TemporaryBinaryFile()
    {
        std::error_code error;
        std::filesystem::remove(path_, error);
    }

    TemporaryBinaryFile(const TemporaryBinaryFile&) = delete;
    TemporaryBinaryFile& operator=(const TemporaryBinaryFile&) = delete;

    [[nodiscard]] const std::filesystem::path& path() const
    {
        return path_;
    }

private:
    std::filesystem::path path_;
};

TEST(RawIqReaderTest, DecodesInterleavedComponents)
{
    std::vector<unsigned char> bytes;
    append_sample(bytes, 16384, -8192);
    const TemporaryBinaryFile file{"cpp_rf_signal_toolkit_components.iq", bytes};
    RawIqReader reader{file.path(), RawIqFormat::signed_int16_little_endian};
    std::array<IqSample, 1> samples{};

    ASSERT_EQ(reader.read_samples(samples), 1U);
    EXPECT_FLOAT_EQ(samples[0].real(), 0.5F);
    EXPECT_FLOAT_EQ(samples[0].imag(), -0.25F);
}

TEST(RawIqReaderTest, NormalizesSignedInt16Extrema)
{
    std::vector<unsigned char> bytes;
    append_sample(bytes, -32768, 32767);
    const TemporaryBinaryFile file{"cpp_rf_signal_toolkit_extrema.iq", bytes};
    RawIqReader reader{file.path(), RawIqFormat::signed_int16_little_endian};
    std::array<IqSample, 1> samples{};

    ASSERT_EQ(reader.read_samples(samples), 1U);
    EXPECT_FLOAT_EQ(samples[0].real(), -1.0F);
    EXPECT_FLOAT_EQ(samples[0].imag(), 32767.0F / 32768.0F);
}

TEST(RawIqReaderTest, DecodesBigEndianInterleavedComponents)
{
    std::vector<unsigned char> bytes;
    append_big_endian_sample(bytes, 16384, -8192);
    const TemporaryBinaryFile file{"cpp_rf_signal_toolkit_big_endian_components.iq", bytes};
    RawIqReader reader{file.path(), RawIqFormat::signed_int16_big_endian};
    std::array<IqSample, 1> samples{};

    ASSERT_EQ(reader.read_samples(samples), 1U);
    EXPECT_FLOAT_EQ(samples[0].real(), 0.5F);
    EXPECT_FLOAT_EQ(samples[0].imag(), -0.25F);
}

TEST(RawIqReaderTest, NormalizesBigEndianSignedInt16Extrema)
{
    std::vector<unsigned char> bytes;
    append_big_endian_sample(bytes, -32768, 32767);
    const TemporaryBinaryFile file{"cpp_rf_signal_toolkit_big_endian_extrema.iq", bytes};
    RawIqReader reader{file.path(), RawIqFormat::signed_int16_big_endian};
    std::array<IqSample, 1> samples{};

    ASSERT_EQ(reader.read_samples(samples), 1U);
    EXPECT_FLOAT_EQ(samples[0].real(), -1.0F);
    EXPECT_FLOAT_EQ(samples[0].imag(), 32767.0F / 32768.0F);
}

TEST(RawIqReaderTest, StreamsBigEndianSamplesAcrossCallerOwnedBuffers)
{
    std::vector<unsigned char> bytes;
    append_big_endian_sample(bytes, 3276, -3276);
    append_big_endian_sample(bytes, 6553, -6553);
    append_big_endian_sample(bytes, 9830, -9830);
    const TemporaryBinaryFile file{"cpp_rf_signal_toolkit_big_endian_streaming.iq", bytes};
    RawIqReader reader{file.path(), RawIqFormat::signed_int16_big_endian};
    std::array<IqSample, 2> samples{};

    ASSERT_EQ(reader.read_samples(samples), 2U);
    EXPECT_FLOAT_EQ(samples[0].real(), 3276.0F / 32768.0F);
    EXPECT_FLOAT_EQ(samples[1].real(), 6553.0F / 32768.0F);

    ASSERT_EQ(reader.read_samples(samples), 1U);
    EXPECT_FLOAT_EQ(samples[0].real(), 9830.0F / 32768.0F);
    EXPECT_EQ(reader.read_samples(samples), 0U);
}

TEST(RawIqReaderTest, StreamsAcrossCallerOwnedBuffers)
{
    std::vector<unsigned char> bytes;
    append_sample(bytes, 3276, -3276);
    append_sample(bytes, 6553, -6553);
    append_sample(bytes, 9830, -9830);
    const TemporaryBinaryFile file{"cpp_rf_signal_toolkit_streaming.iq", bytes};
    RawIqReader reader{file.path(), RawIqFormat::signed_int16_little_endian};
    std::array<IqSample, 2> samples{};

    ASSERT_EQ(reader.read_samples(samples), 2U);
    EXPECT_FLOAT_EQ(samples[0].real(), 3276.0F / 32768.0F);
    EXPECT_FLOAT_EQ(samples[1].real(), 6553.0F / 32768.0F);

    ASSERT_EQ(reader.read_samples(samples), 1U);
    EXPECT_FLOAT_EQ(samples[0].real(), 9830.0F / 32768.0F);
    EXPECT_EQ(reader.read_samples(samples), 0U);
}

TEST(RawIqReaderTest, EmptyOutputDoesNotConsumeInput)
{
    std::vector<unsigned char> bytes;
    append_sample(bytes, 4096, -4096);
    const TemporaryBinaryFile file{"cpp_rf_signal_toolkit_empty_output.iq", bytes};
    RawIqReader reader{file.path(), RawIqFormat::signed_int16_little_endian};

    EXPECT_EQ(reader.read_samples(std::span<IqSample>{}), 0U);

    std::array<IqSample, 1> samples{};
    ASSERT_EQ(reader.read_samples(samples), 1U);
    EXPECT_FLOAT_EQ(samples[0].real(), 0.125F);
    EXPECT_FLOAT_EQ(samples[0].imag(), -0.125F);
}

TEST(RawIqReaderTest, RejectsMissingFile)
{
    const auto path = std::filesystem::temp_directory_path() / "cpp_rf_signal_toolkit_missing.iq";
    std::error_code error;
    std::filesystem::remove(path, error);

    EXPECT_THROW(RawIqReader(path, RawIqFormat::signed_int16_little_endian), std::runtime_error);
}

TEST(RawIqReaderTest, RejectsIncompleteFinalSample)
{
    std::vector<unsigned char> bytes;
    append_sample(bytes, 1024, -1024);
    bytes.push_back(0x7FU);
    const TemporaryBinaryFile file{"cpp_rf_signal_toolkit_incomplete.iq", bytes};
    RawIqReader reader{file.path(), RawIqFormat::signed_int16_little_endian};
    std::array<IqSample, 1> samples{};

    ASSERT_EQ(reader.read_samples(samples), 1U);
    EXPECT_THROW(static_cast<void>(reader.read_samples(samples)), std::runtime_error);
}

TEST(RawIqReaderTest, RejectsUnknownFormat)
{
    const TemporaryBinaryFile file{"cpp_rf_signal_toolkit_unknown_format.iq", {}};
    const auto unknown_format = static_cast<RawIqFormat>(999);

    EXPECT_THROW(RawIqReader(file.path(), unknown_format), std::invalid_argument);
}

}  // namespace
}  // namespace cpp_rf_signal_toolkit
