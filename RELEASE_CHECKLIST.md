# Release Quality Standards - Checklist

**Package:** sobol v1.0.0 **Target:** CRAN Release **Status:** In
Progress **Last Updated:** 2026-04-23

This checklist tracks all critical workstreams for finalizing and
polishing the package for broader use and potential CRAN release.

## Sub-Issue 1: Core Code Verification and R Wrapper Implementation

**Status:** ✅ Mostly Complete \| **Priority:** Low

### Implementation Review

Core C++ library with Rcpp integration working

[`sobol_design()`](https://alrobles.github.io/sobol/reference/sobol_design.md)
wrapper implemented and tested

[`sobol_points()`](https://alrobles.github.io/sobol/reference/sobol_points.md)
wrapper implemented and tested

[`sobol_generator()`](https://alrobles.github.io/sobol/reference/sobol_generator.md)
constructor implemented

[`sobol_next()`](https://alrobles.github.io/sobol/reference/sobol_next.md)
single point generation working

[`sobol_next_n()`](https://alrobles.github.io/sobol/reference/sobol_next_n.md)
batch generation working

[`sobol_skip_to()`](https://alrobles.github.io/sobol/reference/sobol_skip_to.md)
skip functionality working

[`sobol_index()`](https://alrobles.github.io/sobol/reference/sobol_index.md)
state query working

[`sobol_dimensions()`](https://alrobles.github.io/sobol/reference/sobol_dimensions.md)
dimension query working

### Code Quality Review

Review all R wrapper functions for parameter naming consistency

Verify error messages are user-friendly

Ensure all functions follow tidyverse style guide (80 char lines)

Check edge case handling in all wrappers

Verify NULL and missing parameter handling

Test with extreme values (dimension = 1, dimension = 1000)

Run lintr on all R source files

Run styler to ensure consistent formatting

Review for any hardcoded magic numbers

Check for proper use of checkmate assertions

### Input Validation

Using checkmate for input validation

Verify all numeric inputs validated for range

Verify all integer inputs validated

Check dimension parameter bounds (1-1000)

Check skip parameter bounds

Check n parameter validation in batch functions

Verify informative error messages for all validations

## Sub-Issue 2: Documentation Cleanup and Enhancement

**Status:** 🟡 Needs Improvement \| **Priority:** High

### Man Pages Review

Review `sobol_design.Rd` - add performance notes

Review `sobol_points.Rd` - add complexity info

Review `sobol_generator.Rd` - add state management details

Add cross-references between related functions

Add “See Also” sections to all man pages

Document return value formats explicitly

Add notes about computational complexity

Fix typo: “valiting” → “validating” in package description

### Function Documentation Enhancement

Document
[`sobol_design()`](https://alrobles.github.io/sobol/reference/sobol_design.md)
skip parameter behavior

Add examples showing parallel workflows

Document memory usage characteristics

Add performance comparison notes

Link to relevant vignettes from function docs

Add warnings about first point being zeros

Document reproducibility guarantees

Add examples with error handling

### README Improvements

Fix typo: “Test valiting” → “Test validating”

Add badges (CRAN version, R-CMD-check, codecov)

Add performance comparison plot/table

Clarify installation instructions for CRAN release

Add citation information

Update examples to show best practices

Add troubleshooting section

Verify all examples run without errors

### Package-Level Documentation

Review `sobol-package.R` documentation

Add package-level examples

Document design philosophy

Add references to key papers

Create CITATION file for academic use

## Sub-Issue 3: Test Suite Reorganization and Consolidation

**Status:** 🟡 Good Coverage, Needs Organization \| **Priority:** Medium

### Test Coverage Analysis

C++ test suite comprehensive (28 functions)

R test suite comprehensive (77 test cases)

Run covr::package_coverage() and review results

Identify any untested code paths

Add tests for recently added features

Verify all exported functions have tests

Check test coverage for error conditions

### Test Organization

Review `test-sobol_enhanced.R` (515 lines) - consider splitting

Group tests by functionality

Add descriptive test names following pattern: “function_name - scenario”

Add comments explaining complex test logic

Ensure test independence (no global state)

Order tests from simple to complex

Remove any redundant tests

Consolidate similar test patterns

### Integration Tests

Add end-to-end workflow tests

Test
[`sobol_design()`](https://alrobles.github.io/sobol/reference/sobol_design.md)
→ use in optimization

Test parallel generation with multiple workers

Test state persistence and restoration

Test interoperability with common R packages

Add regression tests for bug fixes

### Platform-Specific Tests

Verify tests pass on Windows

Verify tests pass on macOS

Verify tests pass on Linux

Test with R 4.2, 4.3, 4.4

Check for platform-specific floating point issues

Test with different compiler flags

## Sub-Issue 4: Package Metadata Review and CRAN Compliance

**Status:** 🟡 Good Foundation, Final Review Needed \| **Priority:**
High

### DESCRIPTION File Review

Package name valid

Version number appropriate (1.0.0)

Title in title case

Review Description field punctuation

Ensure package references use single quotes

Verify DOI formatting (Bratley & Fox, Joe & Kuo)

Check URL accessibility

Verify BugReports URL

Review <Authors@R> roles (aut, cre, ctb, cph)

Check Encoding is UTF-8

Verify R dependency version (\>= 3.5.0)

Review Imports versions

Check Suggests packages are necessary

### NAMESPACE Review

Generated by roxygen2

Verify all exports are intentional

Check S3 methods properly declared

Verify imports from other packages

Check useDynLib declaration

Ensure no manual edits to NAMESPACE

### .Rbuildignore Review

Verify development files excluded (.github, .Rproj, etc.)

Check large files excluded (benchmarks output, etc.)

Verify git files excluded (.git, .gitignore, .gitattributes)

Check CRAN-SUBMISSION included correctly

Verify documentation source files handled properly

### License Compliance

LICENSE declared: GPL (\>= 3)

Verify LICENSE.md formatting

Check for any third-party code licensing

Verify direction numbers license acknowledgment

Check all contributors acknowledged

### CRAN Automated Checks

Run `R CMD check --as-cran` locally (0 NOTEs, 0 WARNINGs, 0 ERRORs)

Run `rhub::check_for_cran()` on all platforms

Run `devtools::check_win_devel()`

Run `devtools::check_win_release()`

Run `devtools::check_win_oldrelease()`

Check package size \< 5MB

Verify no non-standard directories

Check for compiled code warnings

Verify examples run \< 5 seconds each

Check for any [`.Internal()`](https://rdrr.io/r/base/Internal.html) or
[`.Call()`](https://rdrr.io/r/base/CallExternal.html) usage issues

### URLs and Links

Test GitHub repository URL

Test pkgdown site URL

Test issue tracker URL

Verify all DOI links resolve

Check for any broken links in documentation

Ensure stable URLs (avoid redirects)

## Sub-Issue 5: Add Vignettes and User-Facing Usage Examples

**Status:** ✅ Vignettes Exist \| 🟡 Enhancement Opportunities \|
**Priority:** Medium

### Existing Vignettes Review

`sobol.Rmd` - Getting Started guide exists

`sobol-sequences.Rmd` - Technical introduction exists

Verify all vignette code chunks execute without errors

Check vignette build time (should be \< 60 seconds)

Review plots and figures for quality

Add figure captions where missing

Check for typos and grammar

Verify examples are reproducible

### Vignette Enhancement

Add Quick Start section (\< 5 minutes)

Expand parallel workflows section with concrete example

Add troubleshooting common issues

Include performance tuning tips

Add comparison with base R sampling methods

Show integration with optimization packages

Add real-world use case examples

### New Vignette Opportunities

“Performance Benchmarks” - comprehensive comparison

“Sensitivity Analysis” - practical Sobol indices example

“Advanced Topics” - parallel processing, state management

“Comparison Guide” - vs. LHS, random, other QMC methods

### Example Quality

`inst/examples/usage_examples.R` exists

Review all examples for clarity

Ensure examples follow best practices

Add error handling examples

Show input validation examples

Add commenting to complex examples

Test all examples run successfully

Add examples showing common pitfalls

### Vignette Index

Create vignette index if multiple vignettes

Link vignettes from README

Cross-reference vignettes

Add “Next Steps” at end of each vignette

## Sub-Issue 6: Benchmarking and Performance Documentation

**Status:** 🟡 Infrastructure Present, Documentation Needed \|
**Priority:** Medium

### Benchmark Infrastructure

`inst/benchmarks/compare_sobol_vs_pomp.R` exists

`inst/benchmarks/profile_operations.R` exists

`inst/benchmarks/README.md` documents infrastructure

Run all benchmark scripts and capture results

Update benchmark README with latest results

Add timestamp to benchmark results

Document system specifications used for benchmarks

### Performance Documentation

Create performance comparison table

Add benchmark results to main README

Document time complexity for each function

Document space complexity

Add performance section to primary vignette

Create performance plots (dimension vs. time)

Document scalability characteristics

Add memory usage documentation

### Comparison Benchmarks

Benchmark against pomp::sobol_design

Benchmark against randtoolbox::sobol

Compare with base R random sampling

Compare with Latin Hypercube sampling

Document speedup factors

Create visualization of comparisons

### Performance Targets Review

Document current performance baseline

Track progress toward 10x speedup goal

Document compiler optimization flags used

Test with different optimization levels

Profile for bottlenecks

Consider SIMD optimizations (future)

### Platform-Specific Performance

Benchmark on Linux

Benchmark on macOS

Benchmark on Windows

Document any platform differences

Test on different CPU architectures

## Additional Quality Tasks

### Code Style

Run `lintr::lint_package()` and address issues

Run `styler::style_pkg()` for consistent formatting

Verify 80-character line limit compliance

Check for consistent naming conventions

Remove any commented-out code

Remove TODO/FIXME comments or create issues

### Spell Checking

Run `spelling::spell_check_package()`

Add technical terms to WORDLIST

Fix typos in documentation

Fix typos in comments

Fix typos in vignettes

Review README for spelling errors

### NEWS.md

Create or update NEWS.md

Document all changes for v1.0.0

Follow NEWS.md conventions

Include bug fixes

Include new features

Include breaking changes (if any)

### CITATION File

Create inst/CITATION file

Include proper citation format

Add DOI if available

Reference key papers

Add author information

### Repository Files

Update or create CONTRIBUTING.md

Add CODE_OF_CONDUCT.md

Create .github/ISSUE_TEMPLATE/

Create .github/PULL_REQUEST_TEMPLATE.md

Update README badges

### pkgdown Website

Review `_pkgdown.yml` configuration

Organize reference by topic

Add custom theme if desired

Add news page from NEWS.md

Add articles from vignettes

Deploy and verify site accessibility

## Final Pre-Release Checklist

### Code Freeze

All planned features implemented

All bugs fixed

No outstanding critical issues

Code review completed

### Documentation Freeze

All functions documented

All vignettes complete

README finalized

NEWS.md updated

### Testing Complete

All tests passing

Coverage \> 80%

Platform tests passing

R version tests passing

### CRAN Submission Ready

R CMD check clean (0/0/0)

rhub checks passing

winbuilder checks passing

cran-comments.md prepared

Version number finalized

CRAN-SUBMISSION updated

### Release Preparation

Tag version in git

Create GitHub release

Prepare announcement

Submit to CRAN

Monitor submission status

------------------------------------------------------------------------

**Progress:** - Core Code: ~90% complete - Documentation: ~60%
complete - Tests: ~80% complete - Metadata: ~75% complete - Vignettes:
~70% complete - Benchmarks: ~50% complete

**Overall:** ~70% to release-ready state **Next Steps:** Focus on
documentation cleanup and CRAN compliance checks

**Last Review:** 2026-04-23 **Next Review:** 2026-04-30
