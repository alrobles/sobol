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

#ifndef SOBOL_PRIMITIVE_POLYNOMIAL_HPP
#define SOBOL_PRIMITIVE_POLYNOMIAL_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace sobol {
namespace detail {

inline int degree(std::uint64_t p) {
  int d = -1;
  while (p != 0u) {
    ++d;
    p >>= 1u;
  }
  return d;
}

inline std::uint64_t polynomial_mod(std::uint64_t a, std::uint64_t mod) {
  const int mod_degree = degree(mod);
  while (a != 0u && degree(a) >= mod_degree) {
    a ^= (mod << static_cast<unsigned>(degree(a) - mod_degree));
  }
  return a;
}

inline std::uint32_t multiply_mod(std::uint32_t a, std::uint32_t b,
                                  std::uint32_t primitive_poly,
                                  int primitive_degree) {
  std::uint32_t result = 0u;
  std::uint32_t x = a;
  std::uint32_t y = b;
  while (y != 0u) {
    if ((y & 1u) != 0u) {
      result ^= x;
    }
    y >>= 1u;
    const bool overflow = ((x >> static_cast<unsigned>(primitive_degree - 1)) & 1u) != 0u;
    x <<= 1u;
    if (overflow) {
      x ^= primitive_poly;
    }
    x &= ((1u << static_cast<unsigned>(primitive_degree)) - 1u);
  }
  return result;
}

inline std::uint32_t pow_mod(std::uint32_t base, std::uint64_t exp,
                             std::uint32_t primitive_poly,
                             int primitive_degree) {
  std::uint32_t result = 1u;
  std::uint32_t value = base;
  std::uint64_t e = exp;
  while (e != 0u) {
    if ((e & 1u) != 0u) {
      result = multiply_mod(result, value, primitive_poly, primitive_degree);
    }
    e >>= 1u;
    value = multiply_mod(value, value, primitive_poly, primitive_degree);
  }
  return result;
}

inline std::vector<std::uint32_t> prime_factors(std::uint32_t n) {
  std::vector<std::uint32_t> factors;
  std::uint32_t remaining = n;
  for (std::uint32_t d = 2u; d * d <= remaining; ++d) {
    if (remaining % d == 0u) {
      factors.push_back(d);
      while (remaining % d == 0u) {
        remaining /= d;
      }
    }
  }
  if (remaining > 1u) {
    factors.push_back(remaining);
  }
  return factors;
}

inline bool is_irreducible(std::uint32_t poly) {
  const int d = degree(poly);
  if (d <= 0) {
    return false;
  }

  for (int div_degree = 1; div_degree <= d / 2; ++div_degree) {
    const std::uint32_t start = (1u << static_cast<unsigned>(div_degree)) | 1u;
    const std::uint32_t end = (1u << static_cast<unsigned>(div_degree + 1));
    for (std::uint32_t divisor = start; divisor < end; divisor += 2u) {
      if (degree(divisor) != div_degree) {
        continue;
      }
      if (polynomial_mod(poly, divisor) == 0u) {
        return false;
      }
    }
  }
  return true;
}

inline bool is_primitive(std::uint32_t poly) {
  const int d = degree(poly);
  if (d <= 0 || ((poly & 1u) == 0u) || !is_irreducible(poly)) {
    return false;
  }

  const std::uint32_t order = (1u << static_cast<unsigned>(d)) - 1u;
  const auto factors = prime_factors(order);

  for (const std::uint32_t factor : factors) {
    if (pow_mod(2u, order / factor, poly, d) == 1u) {
      return false;
    }
  }
  return true;
}

inline std::vector<std::uint32_t> generate_primitive_polynomials(std::size_t dimensions) {
  if (dimensions == 0u) {
    return {};
  }

  std::vector<std::uint32_t> result;
  result.reserve(dimensions);
  result.push_back(0b11u);  // x + 1 (dimension 1)

  if (dimensions == 1u) {
    return result;
  }

  for (int d = 2; d <= 30 && result.size() < dimensions; ++d) {
    const std::uint32_t start = (1u << static_cast<unsigned>(d)) | 1u;
    const std::uint32_t end = (1u << static_cast<unsigned>(d + 1));
    for (std::uint32_t poly = start; poly < end && result.size() < dimensions; poly += 2u) {
      if (is_primitive(poly)) {
        result.push_back(poly);
      }
    }
  }

  if (result.size() < dimensions) {
    throw std::runtime_error("unable to generate enough primitive polynomials for requested dimensions");
  }

  return result;
}

}  // namespace detail
}  // namespace sobol

#endif  // SOBOL_PRIMITIVE_POLYNOMIAL_HPP
