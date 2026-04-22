#ifndef SOBOL_DIRECTION_NUMBERS_HPP
#define SOBOL_DIRECTION_NUMBERS_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "sobol/primitive_polynomial.hpp"

namespace sobol {
namespace detail {

constexpr std::size_t kSobolBits = 32u;

inline bool is_full_rank(std::vector<std::uint32_t> rows, std::size_t width) {
  std::size_t rank = 0u;
  for (std::size_t bit = 0u; bit < width && rank < rows.size(); ++bit) {
    std::size_t pivot = rank;
    while (pivot < rows.size() && (((rows[pivot] >> static_cast<unsigned>(bit)) & 1u) == 0u)) {
      ++pivot;
    }
    if (pivot == rows.size()) {
      continue;
    }
    std::swap(rows[rank], rows[pivot]);
    for (std::size_t i = 0u; i < rows.size(); ++i) {
      if (i != rank && (((rows[i] >> static_cast<unsigned>(bit)) & 1u) != 0u)) {
        rows[i] ^= rows[rank];
      }
    }
    ++rank;
  }
  return rank == rows.size();
}

inline std::vector<std::uint32_t> enforce_initial_numbers_full_rank(
    std::size_t degree, std::uint32_t seed) {
  std::vector<std::uint32_t> m(degree, 1u);

  for (std::size_t i = 1u; i <= degree; ++i) {
    const std::uint32_t upper_bit = (1u << static_cast<unsigned>(i - 1u));
    const std::uint32_t mask = (1u << static_cast<unsigned>(i)) - 1u;
    // Cycle through seed chunks to derive deterministic per-bit candidates.
    const std::uint32_t candidate =
        ((seed >> static_cast<unsigned>((i - 1u) % 16u)) | upper_bit | 1u) & mask;

    bool accepted = false;
    for (std::uint32_t odd = candidate | 1u; odd <= mask; odd += 2u) {
      const std::uint32_t with_top = odd | upper_bit;
      m[i - 1u] = with_top;
      std::vector<std::uint32_t> prefix(m.begin(), m.begin() + static_cast<std::ptrdiff_t>(i));
      if (is_full_rank(prefix, i)) {
        accepted = true;
        break;
      }
    }

    if (!accepted) {
      for (std::uint32_t odd = 1u; odd <= mask; odd += 2u) {
        const std::uint32_t with_top = odd | upper_bit;
        m[i - 1u] = with_top;
        std::vector<std::uint32_t> prefix(m.begin(), m.begin() + static_cast<std::ptrdiff_t>(i));
        if (is_full_rank(prefix, i)) {
          accepted = true;
          break;
        }
      }
    }

    if (!accepted) {
      throw std::runtime_error("failed to find full-rank initial direction numbers");
    }
  }

  return m;
}

inline std::array<std::uint32_t, kSobolBits> direction_numbers_for_dimension(
    std::size_t dimension_index, const std::vector<std::uint32_t>& primitive_polynomials) {
  if (dimension_index == 0u) {
    throw std::invalid_argument("dimension_index must be >= 1 (1-based indexing)");
  }
  if (dimension_index > primitive_polynomials.size()) {
    throw std::invalid_argument("missing primitive polynomial for requested dimension");
  }

  std::array<std::uint32_t, kSobolBits> v{};

  if (dimension_index == 1u) {
    for (std::size_t i = 0u; i < kSobolBits; ++i) {
      v[i] = (1u << static_cast<unsigned>(31u - i));
    }
    return v;
  }

  const std::uint32_t poly = primitive_polynomials[dimension_index - 1u];
  const std::size_t degree = static_cast<std::size_t>(detail::degree(poly));
  // Mix polynomial bits with a small odd constant to decorrelate per-dimension seeds.
  const auto m =
      enforce_initial_numbers_full_rank(degree, poly ^ static_cast<std::uint32_t>(dimension_index * 0x9E37u));

  for (std::size_t i = 1u; i <= degree; ++i) {
    v[i - 1u] = m[i - 1u] << static_cast<unsigned>(kSobolBits - i);
  }

  for (std::size_t i = degree + 1u; i <= kSobolBits; ++i) {
    std::uint32_t value = v[i - degree - 1u] ^ (v[i - degree - 1u] >> static_cast<unsigned>(degree));
    for (std::size_t k = 1u; k < degree; ++k) {
      if (((poly >> static_cast<unsigned>(k)) & 1u) != 0u) {
        value ^= v[i - k - 1u];
      }
    }
    v[i - 1u] = value;
  }

  return v;
}

inline std::vector<std::array<std::uint32_t, kSobolBits>> build_direction_table(std::size_t dimensions) {
  const auto primitive_polys = generate_primitive_polynomials(dimensions);
  std::vector<std::array<std::uint32_t, kSobolBits>> table;
  table.reserve(dimensions);
  for (std::size_t d = 1u; d <= dimensions; ++d) {
    table.push_back(direction_numbers_for_dimension(d, primitive_polys));
  }
  return table;
}

}  // namespace detail
}  // namespace sobol

#endif  // SOBOL_DIRECTION_NUMBERS_HPP
