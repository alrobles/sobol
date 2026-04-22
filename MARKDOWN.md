# Introduction

Sobol' sequences [@Sobol1967] are a cornerstone of quasi-Monte Carlo
methods. Their construction relies on:

1.  A set of primitive polynomials over the finite field $\mathbb{Z}_2$,
    one per dimension.

2.  A recurrence to generate *direction numbers* (binary fractions) from
    each polynomial.

3.  An efficient Gray-code update to produce successive points.

The seminal implementation by Bratley and Fox [@BratleyFox1988]
(Algorithm 659) provided tables for up to 40 dimensions; Joe and Kuo
[@JoeKuo2003] extended these to 1111 dimensions while enforcing Sobol's
Property A. Our goal is to re-derive every step from the mathematical
definitions, implement the generation of polynomials and direction
numbers algorithmically, and package the result as a modern C++17
header-only library that can be easily wrapped for R.

# Mathematical Foundations

## Primitive Polynomials over $\mathbb{Z}_2$

A polynomial
$$P(x) = x^d + a_{d-1} x^{d-1} + \cdots + a_1 x + 1,\quad a_i \in \{0,1\},$$
is *primitive* if it is irreducible and its roots generate the
multiplicative group of the extension field $\mathbb{F}_{2^d}$.
Equivalently, the smallest $k>0$ for which $P(x)$ divides $x^k + 1$ is
$k = 2^d - 1$.

The number of primitive polynomials of degree $d$ is $\phi(2^d - 1)/d$,
where $\phi$ is Euler's totient function.

For Sobol' sequences, we require one distinct primitive polynomial for
each dimension $j \ge 2$ (dimension 1 uses the special van der Corput
sequence, corresponding to $P(x) \equiv x+1$ with all direction numbers
$m_{k,1}=1$).

## Direction Numbers and Recurrence

For dimension $j$ with primitive polynomial of degree $d_j$, the
direction numbers $v_{k,j} = m_{k,j}/2^k$ are defined by integers
$m_{k,j}$ satisfying:

-   For $1 \le k \le d_j$: $m_{k,j}$ is an odd integer $< 2^k$, chosen
    arbitrarily (but see
    §[\[sec:propertyA\]](#sec:propertyA){reference-type="ref"
    reference="sec:propertyA"}).

-   For $k > d_j$: $$\begin{aligned}
    m_{k,j} &= 2 a_1 m_{k-1,j} \;\oplus\; 2^2 a_2 m_{k-2,j} \;\oplus\; \cdots \;\oplus\; 2^{d_j-1} a_{d_j-1} m_{k-d_j+1,j} \nonumber\\
    &\quad \oplus\; 2^{d_j} m_{k-d_j,j} \;\oplus\; m_{k-d_j,j},
    \end{aligned}$$ where $a_i$ are the coefficients of $P(x)$ (with
    $a_{d_j}=1$ implicit), and $\oplus$ denotes bitwise XOR.

## Gray-Code Point Generation

Let $n$ be the point index (starting from 0). Write $n$ in binary:
$n = \sum_{\ell} b_\ell 2^\ell$. The original Sobol' method computes the
$j$-th coordinate as
$$x_n^{(j)} = b_1 v_{1,j} \oplus b_2 v_{2,j} \oplus \cdots$$ Antonov and
Saleev [@AntonovSaleev1979] showed that replacing the binary digits
$b_\ell$ with the Gray code $g_\ell$ (where
$g_\ell = b_\ell \oplus b_{\ell+1}$) leaves discrepancy unchanged. The
Gray code of $n$ and $n+1$ differ in exactly one bit: the position $c$
of the rightmost zero bit in $n$. Thus,
$$x_{n+1}^{(j)} = x_n^{(j)} \oplus v_{c+1,j}.$$ This yields an $O(1)$
per-dimension update (after finding $c$).

## Property A []{#sec:propertyA label="sec:propertyA"}

Sobol' [@Sobol1976] introduced an additional uniformity condition: a
$d$-dimensional sequence has Property A if within every block of $2^d$
points, each of the $2^d$ subcubes obtained by bisecting each axis
contains exactly one point. This is equivalent to
$$\det(V_d) \equiv 1 \pmod 2,$$ where $V_d$ is the $d \times d$ binary
matrix whose $(i,j)$ entry is the first bit after the binary point of
$v_{i,j}$ (i.e., the most significant fractional bit of $v_{i,j}$, which
is $1$ if $m_{i,j} \ge 2^{i-1}$ and $0$ otherwise).

Joe and Kuo [@JoeKuo2003] generated initial $m_{k,j}$ values that
satisfy Property A for all $d \le 1111$ by starting with random odd
values and iteratively adjusting them until the determinant condition
holds. We will implement a similar procedure.

# Generating Tables from Scratch

## Primitive Polynomials

To generate all primitive polynomials up to a given degree (say 13,
which suffices for 1111 dimensions), we can:

1.  Enumerate all polynomials
    $x^d + a_{d-1}x^{d-1} + \cdots + a_1 x + 1$ with $a_i \in \{0,1\}$.

2.  Test each for primitivity using standard algorithms (e.g., check
    that $x^{2^d-1} \equiv 1 \pmod{P(x)}$ and that for every prime
    factor $q$ of $2^d-1$, $x^{(2^d-1)/q} \not\equiv 1 \pmod{P(x)}$).
    Polynomial arithmetic modulo $P(x)$ can be implemented efficiently
    using bitwise operations.

3.  Sort the primitive polynomials by degree, and within each degree by
    their integer representation (e.g., as in Joe and Kuo).

This can be done at compile time (using `constexpr` functions) or via a
separate code generator that produces a header file with the polynomial
coefficients.

## Initial Direction Numbers with Property A

For each dimension $j \ge 2$, we need $d_j$ initial $m_{k,j}$ values
($k=1,\dots,d_j$) that are odd and $< 2^k$. To enforce Property A up to
a target dimension $D$, we follow Joe & Kuo's iterative method:

1.  For $j=2,\dots,D$:

    1.  Generate random odd integers $m_{k,j} < 2^k$ for
        $k=1,\dots,d_j$.

    2.  Form the $j \times j$ matrix $V_j$ using the first bit of
        $v_{k,\ell}$ for $\ell=1,\dots,j$ and $k=1,\dots,j$ (direction
        numbers beyond $d_\ell$ are obtained via the recurrence).

    3.  Compute $\det(V_j) \bmod 2$. If it is 0, modify one of the
        initial $m_{k,j}$ (by adding $2^{k-1}$ modulo $2^k$, which flips
        the first bit) and recompute until $\det(V_j) \equiv 1 \pmod 2$.

The determinant calculation is done over $\mathbb{Z}_2$ using Gaussian
elimination (XOR operations). To make this efficient for large $D$, we
maintain the row-reduced form of $V_{j-1}$ and only process the new row.

This procedure can be executed offline; the resulting initial $m_{k,j}$
can be stored as a static constexpr array in the header.

## Expanding Direction Numbers to Required Precision

For a given maximum number of points $N$, we need direction numbers up
to $K = \lceil \log_2(N+1) \rceil$ bits. For each dimension $j$, we use
the recurrence to compute $m_{k,j}$ for $k = d_j+1,\dots,K$. These
values are stored in a 2D array `m[K][dim]` (or a flat array for cache
efficiency).

# Core Algorithm Implementation

## Finding the Rightmost Zero Bit

The index $c$ (0-based) of the least-significant zero bit in $n$ can be
computed efficiently using compiler intrinsics (e.g., `__builtin_ctz` on
GCC/Clang) or a small lookup table based on de Bruijn sequences. Since
$n$ is a 32-bit or 64-bit integer, this is $O(1)$.

## Point Update

For each dimension $i$:

-   Maintain an integer `x[i]` representing the fixed-point coordinate
    with `b[i]` fractional bits (initially 0).

-   Maintain the current number of fractional bits `b[i]`.

-   When $c$ is found:

    -   If `b[i] >= c`: `x[i] ^= m[c][i] << (b[i] - c)`.

    -   Else: `x[i] = (x[i] << (c - b[i])) ^ m[c][i]`, and `b[i] = c`.

-   The floating-point coordinate is `x[i] / (1ULL << (b[i]+1))`.

## Skip and Reset

Skipping $M$ points can be done by simply advancing the generator $M$
times (acceptable for moderate $M$). For large skips, a more
sophisticated block-skip algorithm could be implemented, but it is not
essential for the first version. Reset sets `n=0`, `x[i]=0`, `b[i]=0`.

# Header-Only C++17 Library Design

## Template Parameters

We provide a template class parameterized by the maximum dimension `D`
(default 1111) and the number of bits `Bits` (default 32). This allows
compile-time allocation of arrays using `std::array`, enabling better
optimization.

    template<unsigned D = 1111, unsigned Bits = 32>
    class SobolGenerator {
    public:
        static_assert(D <= MaxSupportedDim, "Dimension exceeds precomputed tables");

        SobolGenerator(unsigned dim) : dim_(dim) {
            if (dim > D) throw std::invalid_argument("Dimension exceeds template parameter");
            initialize();
        }

        void next(std::vector<double>& point);
        void next(double* point);

        void skip(unsigned long long n);
        void reset();

        unsigned dimension() const { return dim_; }
        unsigned long long count() const { return n_; }

    private:
        unsigned dim_;
        unsigned long long n_ = 0;
        std::array<uint32_t, D> x_{};
        std::array<unsigned, D> b_{};
        std::array<std::array<uint32_t, Bits>, D> m_{};  // m[dim][bit]

        void initialize();
        static unsigned rightmost_zero_bit(uint32_t n);
    };

## Compile-Time Table Generation

Where possible, we generate primitive polynomials and initial direction
numbers at compile time using `constexpr` functions. For example, a
`constexpr` function can compute the list of primitive polynomials up to
a given degree. However, due to the complexity of primitivity testing
and the large number of combinations, a more practical approach is to
run a code generator (written in C++ or Python) that outputs a
`constexpr` array of polynomial coefficients and initial $m$ values.
This generated header is then included in the library.

## Memory Layout

The direction numbers `m_[dim][bit]` are stored with `dim` as the first
index and `bit` as the second. During point generation, we access
`m_[i][c]` for each dimension $i$. This is cache-friendly if `dim` is
large.

## Portability and Performance

-   Use `uint32_t` from `<cstdint>` for fixed-width integers.

-   Use `__builtin_ctz` or `std::countr_zero` (C++20) for the rightmost
    zero bit. Provide a fallback implementation using a lookup table.

-   Mark small functions `inline` and `constexpr` where appropriate.

-   The generator is not thread-safe; users must synchronize externally
    or create per-thread instances.

# Porting to R

## Rcpp Interface

We will create an R package that uses **Rcpp** to expose the C++
generator to R. The package will provide:

-   `sobol_points(n, dim, skip = 0)`: returns an $n \times \texttt{dim}$
    matrix of points.

-   `sobol_generator(dim)`: returns an R6 or Rcpp module object that can
    generate points incrementally.

## Example Usage in R

    library(sobol)
    # Generate 1000 points in 10 dimensions
    pts <- sobol_points(1000, 10)
    # Integrate a test function
    f <- function(x) prod(abs(4*x - 2))
    mean(apply(pts, 1, f))  # Should be close to 1

## CRAN Compliance

The package will include the BSD-licensed direction number tables (if
not generated at compile time) and proper attribution to Sobol', Bratley
& Fox, and Joe & Kuo. The C++ code will be original, avoiding any ACM
TOMS code.

# Validation and Testing

## Unit Tests

-   Compare the first few points for dimensions 1--10 against known
    reference values (e.g., from Joe & Kuo's paper or other trusted
    implementations).

-   Verify Property A by checking $\det(V_d) \bmod 2 = 1$ for all $d$ up
    to the maximum supported.

-   Test the discrepancy of generated sequences using known test
    integrals (e.g., $\int \prod_{i=1}^s |4x_i - 2| \, dx = 1$).

## Performance Benchmarks

Measure the time to generate $10^6$ points in dimensions 10, 100, and
1000, and compare against existing implementations (NLopt, Boost).

# Conclusion

We have outlined a complete, from-scratch plan for a modern C++17 Sobol'
sequence generator. By deeply understanding the mathematics and
implementing table generation ourselves, we produce a clean, header-only
library with no external dependencies. The design is optimized for
performance and portability, and includes a clear path to R integration
via Rcpp. This work will provide the scientific computing community with
a transparent, well-documented, and freely reusable Sobol' sequence
implementation.

::: thebibliography
9 I. M. Sobol', "On the distribution of points in a cube and the
approximate evaluation of integrals," *USSR Comput. Math. Math. Phys.*,
vol. 7, no. 4, pp. 86--112, 1967.

I. M. Sobol', "Uniformly distributed sequences with an additional
uniform property," *USSR Comput. Math. Math. Phys.*, vol. 16, no. 5,
pp. 236--242, 1976.

I. A. Antonov and V. M. Saleev, "An economic method of computing
$LP_\tau$-sequences," *USSR Comput. Math. Math. Phys.*, vol. 19, no. 1,
pp. 252--256, 1979.

P. Bratley and B. L. Fox, "Algorithm 659: Implementing Sobol's
quasirandom sequence generator," *ACM Trans. Math. Softw.*, vol. 14,
no. 1, pp. 88--100, 1988.

S. Joe and F. Y. Kuo, "Remark on Algorithm 659: Implementing Sobol's
quasirandom sequence generator," *ACM Trans. Math. Softw.*, vol. 29,
no. 1, pp. 49--57, 2003.
:::
