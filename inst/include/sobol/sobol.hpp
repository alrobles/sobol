// BSD 3-Clause License
//
// Copyright (c) 2025, Angel Robles
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
    if (index_ == exhausted_index()) {
      throw std::overflow_error("Sobol engine index exceeded 32-bit direction number capacity");
    }
    const auto point = current_point();
    if (index_ == max_supported_index()) {
      index_ = exhausted_index();
      return point;
    }
    ++index_;
    const auto bit = trailing_zero_count(index_);
    for (std::size_t d = 0u; d < dimensions_; ++d) {
      state_[d] ^= direction_table_[d][bit];
    }
    return point;
  }

  void skip_to(std::uint64_t point_index) {
    if (point_index > max_supported_index()) {
      throw std::out_of_range("point_index exceeds supported range for 32-bit direction numbers");
    }
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
  static constexpr std::uint64_t max_supported_index() {
    return (std::uint64_t{1} << detail::kSobolBits) - std::uint64_t{1};
  }

  static constexpr std::uint64_t exhausted_index() {
    return max_supported_index() + std::uint64_t{1};
  }

  static std::size_t trailing_zero_count(std::uint64_t value) {
    if (value == 0u) {
      throw std::invalid_argument("trailing_zero_count expects non-zero input");
    }
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
