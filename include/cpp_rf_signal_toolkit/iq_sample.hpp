#pragma once

#include <complex>

namespace cpp_rf_signal_toolkit {

// In-phase values use the real component; quadrature values use the imaginary
// component. Raw file readers convert their source encoding to this type.
using IqSample = std::complex<float>;

}  // namespace cpp_rf_signal_toolkit
