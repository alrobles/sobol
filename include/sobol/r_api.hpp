#ifndef SOBOL_R_API_HPP
#define SOBOL_R_API_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include "sobol/sobol.hpp"

namespace sobol {

// Utility adapter for Rcpp wrappers.
// `as_column_major` returns data in R matrix layout (column-major order).
inline std::vector<double> sobol_points_column_major(std::size_t n, std::size_t dimensions,
                                                     std::uint64_t skip = 0u) {
  const auto points = sobol_points(n, dimensions, skip);
  std::vector<double> out(n * dimensions);
  for (std::size_t col = 0u; col < dimensions; ++col) {
    for (std::size_t row = 0u; row < n; ++row) {
      out[col * n + row] = points[row][col];
    }
  }
  return out;
}

}  // namespace sobol

#endif  // SOBOL_R_API_HPP
