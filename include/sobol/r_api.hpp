#ifndef SOBOL_R_API_HPP
#define SOBOL_R_API_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include "sobol/sobol.hpp"

namespace sobol {

// Utility adapter for Rcpp wrappers.
// `sobol_points_column_major` returns data in R matrix layout (column-major order).
inline std::vector<double> sobol_points_column_major(std::size_t n, std::size_t dimensions,
                                                     std::uint64_t skip = 0u) {
  std::vector<double> out(n * dimensions);
  SobolEngine engine(dimensions, skip);

  for (std::size_t row = 0u; row < n; ++row) {
    const auto point = engine.next();
    for (std::size_t col = 0u; col < dimensions; ++col) {
      out[col * n + row] = point[col];
    }
  }
  return out;
}

}  // namespace sobol

#endif  // SOBOL_R_API_HPP
