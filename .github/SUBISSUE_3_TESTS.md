# Sub-Issue 3: Test Suite Reorganization and Consolidation

**Parent Issue:** Enhancement Plan - Prepare sobol R package for release-quality standards
**Priority:** Medium
**Status:** Good Coverage, Needs Organization
**Assignee:** TBD
**Estimated Effort:** 12-20 hours

## Objective

Clean up and consolidate testing infrastructure while maintaining or improving coverage. Organize tests for maintainability.

## Background

Test coverage is excellent with 77 R test cases and 28 C++ test functions. Some test files are large and could benefit from reorganization.

## Tasks

### Test Organization
- [ ] Review test-sobol_enhanced.R (515 lines) - consider splitting
- [ ] Group tests by functionality
- [ ] Use descriptive test names: "function_name - scenario"
- [ ] Add comments explaining complex test logic
- [ ] Ensure tests are independent (no shared state)
- [ ] Order tests from simple to complex
- [ ] Remove redundant tests
- [ ] Consolidate similar test patterns

### Coverage Analysis
- [ ] Run `covr::package_coverage()` and review
- [ ] Identify untested code paths
- [ ] Add tests for edge cases
- [ ] Verify all exported functions tested
- [ ] Test error conditions
- [ ] Add regression tests for bug fixes

### Integration Tests
- [ ] Add end-to-end workflow tests
- [ ] Test sobol_design() in optimization workflow
- [ ] Test parallel generation scenarios
- [ ] Test state persistence
- [ ] Test with common R packages (tidyverse, purrr, etc.)

### Platform Tests
- [ ] Verify tests pass on Windows
- [ ] Verify tests pass on macOS
- [ ] Verify tests pass on Linux
- [ ] Test with R 4.2, 4.3, 4.4
- [ ] Check floating point consistency across platforms
- [ ] Test with different compiler flags

### Test Documentation
- [ ] Document test strategy
- [ ] Add README to tests/ if needed
- [ ] Document how to run specific test suites
- [ ] Explain test data generation
- [ ] Document expected test run time

## Current Test Files

```
tests/
├── test_sobol.cpp                    # C++ basic tests
├── test_sobol_enhanced.cpp           # C++ comprehensive tests (28 functions)
├── testthat/
│   ├── test-sobol_design.R           # 218 lines, ~30 tests
│   ├── test-sobol_enhanced.R         # 515 lines, ~48 tests
│   └── test-sobol_generator.R        # 295 lines, ~29 tests
└── testthat.R
```

## Proposed Organization

Consider splitting test-sobol_enhanced.R into:
- test-sobol_core.R - Basic functionality
- test-sobol_edge_cases.R - Edge cases and extremes
- test-sobol_integration.R - Integration scenarios
- test-sobol_performance.R - Performance regression tests

## Success Criteria

- [ ] Test coverage ≥ 90%
- [ ] All tests well-organized and documented
- [ ] Tests pass on all platforms
- [ ] Test suite runs in < 60 seconds
- [ ] No redundant tests
- [ ] Clear test failure messages
- [ ] Tests are maintainable

## Dependencies

None - can be done independently

## Tools

- `covr::package_coverage()`
- `testthat::test_package()`
- `devtools::test()`
- GitHub Actions for platform testing

## Notes

Maintain backwards compatibility - don't break existing tests unnecessarily. Focus on organization and clarity.
