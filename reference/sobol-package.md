# sobol: Quasi-Monte Carlo Sobol Sequence Generator

Provides a fast and efficient implementation of Sobol sequences for
quasi-Monte Carlo methods. The Sobol sequence is a low-discrepancy
sequence with the property that for all values of N, its subsequence x1,
..., xN has a low discrepancy. It can be used to generate quasi-random
numbers for use in Monte Carlo integration and other simulation methods.
This implementation is based on the algorithms described by Bratley and
Fox (1988)
[doi:10.1145/42288.214372](https://doi.org/10.1145/42288.214372) and
uses direction numbers from Joe and Kuo (2008)
[doi:10.1145/1358628.1358630](https://doi.org/10.1145/1358628.1358630) .
The package includes both batch and incremental interfaces with support
for arbitrary starting indices and reproducible sequences. It uses
'Rcpp' for efficient 'C++' integration.

## See also

Useful links:

- <https://alrobles.github.io/sobol/>

- Report bugs at <https://github.com/alrobles/sobol/issues>

## Author

**Maintainer**: Angel Robles <a.l.robles.fernandez@gmail.com>

Other contributors:

- Ilya M. Sobol (Original Sobol sequence algorithm) \[contributor\]

- Paul Bratley (Algorithm implementation reference) \[contributor\]

- Bennett L. Fox (Algorithm implementation reference) \[contributor\]

- Stephen Joe (Direction numbers and primitive polynomials)
  \[contributor\]

- Frances Y. Kuo (Direction numbers and primitive polynomials)
  \[contributor\]
