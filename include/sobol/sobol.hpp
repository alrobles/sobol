#ifndef SOBOL_SOBOL_HPP
#define SOBOL_SOBOL_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

#include "sobol/direction_numbers.hpp"

namespace sobol {

class SobolEngine {
 public:
  explicit SobolEngine(std::size_t dimensions, std::uint64_t skip = 0u)
      : dimensions_(dimensions),
        direction_table_(detail::build_direction_table(dimensions)),
        state_(dimensions, 0u),
        index_(0u) {
    if (dimensions_ == 0u) {
      throw std::invalid_argument("dimensions must be greater than zero");
    }
    if (skip != 0u) {
      skip_to(skip);
    }
  }

  std::size_t dimensions() const noexcept { return dimensions_; }
  std::uint64_t index() const noexcept { return index_; }

  std::vector<double> next() {
    const auto point = current_point();
    ++index_;
    const auto bit = rightmost_zero_bit_position(index_);
    for (std::size_t d = 0u; d < dimensions_; ++d) {
      state_[d] ^= direction_table_[d][bit];
    }
    return point;
  }

  void skip_to(std::uint64_t point_index) {
    index_ = point_index;
    const std::uint64_t gray = point_index ^ (point_index >> 1u);

    for (std::size_t d = 0u; d < dimensions_; ++d) {
      std::uint32_t value = 0u;
      for (std::size_t bit = 0u; bit < detail::kSobolBits; ++bit) {
        if (((gray >> bit) & 1u) != 0u) {
          value ^= direction_table_[d][bit];
        }
      }
      state_[d] = value;
    }
  }

  std::vector<double> current_point() const {
    static constexpr double inv_pow2_32 = 1.0 / 4294967296.0;
    std::vector<double> point(dimensions_);
    for (std::size_t d = 0u; d < dimensions_; ++d) {
      point[d] = static_cast<double>(state_[d]) * inv_pow2_32;
    }
    return point;
  }

 private:
  static std::size_t rightmost_zero_bit_position(std::uint64_t value) {
    std::size_t position = 0u;
    while ((value & 1u) == 0u) {
      ++position;
      value >>= 1u;
    }
    if (position >= detail::kSobolBits) {
      throw std::overflow_error("Sobol engine index exceeded 32-bit direction number capacity");
    }
    return position;
  }

  std::size_t dimensions_;
  std::vector<std::array<std::uint32_t, detail::kSobolBits>> direction_table_;
  std::vector<std::uint32_t> state_;
  std::uint64_t index_;
};

inline std::vector<std::vector<double>> sobol_points(std::size_t n, std::size_t dimensions,
                                                      std::uint64_t skip = 0u) {
  SobolEngine engine(dimensions, skip);
  std::vector<std::vector<double>> points;
  points.reserve(n);
  for (std::size_t i = 0u; i < n; ++i) {
    points.push_back(engine.next());
  }
  return points;
}

}  // namespace sobol

#endif  // SOBOL_SOBOL_HPP
