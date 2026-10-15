#include "cpp_rf_signal_toolkit/iq_sample.hpp"

#include <gtest/gtest.h>

namespace cpp_rf_signal_toolkit {
namespace {

TEST(IqSampleTest, MapsInPhaseAndQuadratureToComplexComponents)
{
    const IqSample sample{1.25F, -0.5F};

    EXPECT_FLOAT_EQ(sample.real(), 1.25F);
    EXPECT_FLOAT_EQ(sample.imag(), -0.5F);
}

}  // namespace
}  // namespace cpp_rf_signal_toolkit
