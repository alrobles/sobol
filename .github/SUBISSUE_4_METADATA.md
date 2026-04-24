# Sub-Issue 4: Package Metadata Review and CRAN Compliance

**Parent Issue:** Enhancement Plan - Prepare sobol R package for release-quality standards
**Priority:** High
**Status:** Good Foundation, Final Review Needed
**Assignee:** TBD
**Estimated Effort:** 8-16 hours

## Objective

Ensure package metadata and structure comply with CRAN requirements and best practices. Pass all CRAN automated checks.

## Background

Package has good foundation with CRAN-SUBMISSION file present. This task ensures all metadata is correct and all CRAN policies are followed.

## Tasks

### DESCRIPTION File Review
- [ ] Verify Title is in title case
- [ ] Check Description field punctuation
- [ ] Ensure package references use single quotes ('Rcpp', 'checkmate')
- [ ] Verify DOI formatting for references
- [ ] Test URL accessibility (GitHub and pkgdown)
- [ ] Verify BugReports URL works
- [ ] Review Authors@R roles (aut, cre, ctb, cph)
- [ ] Check dependency versions are appropriate
- [ ] Verify Suggests packages are all necessary
- [ ] Check Encoding is UTF-8

### NAMESPACE Verification
- [ ] Verify all exports are intentional
- [ ] Check S3 methods properly declared
- [ ] Verify imports are minimal and necessary
- [ ] Check useDynLib declaration correct
- [ ] Confirm roxygen2 generated (don't edit manually)

### License Compliance
- [ ] Verify LICENSE.md formatting
- [ ] Check third-party code licensing
- [ ] Verify direction numbers license acknowledgment
- [ ] Ensure all contributors credited
- [ ] Review copyright statements

### .Rbuildignore Review
- [ ] Verify development files excluded (.github/, .Rproj, etc.)
- [ ] Check large files excluded
- [ ] Verify git files excluded
- [ ] Check CRAN-SUBMISSION handling
- [ ] Verify CMakeLists.txt excluded
- [ ] Check benchmark output excluded

### CRAN Automated Checks
- [ ] Run `R CMD check --as-cran` locally
- [ ] Run `rhub::check_for_cran()`
- [ ] Run `devtools::check_win_devel()`
- [ ] Run `devtools::check_win_release()`
- [ ] Run `devtools::check_win_oldrelease()`
- [ ] Verify package size < 5MB
- [ ] Check example run times < 5 seconds each
- [ ] Verify no non-standard directories

### URLs and Links
- [ ] Test GitHub repository URL
- [ ] Test pkgdown site URL
- [ ] Test issue tracker URL
- [ ] Verify all DOI links resolve
- [ ] Check for broken links in docs
- [ ] Ensure URLs are stable (no redirects)

### Additional Files
- [ ] Create NEWS.md with version history
- [ ] Create or verify CITATION file
- [ ] Prepare cran-comments.md for submission
- [ ] Update CRAN-SUBMISSION file

## CRAN Policy Checklist

### Must Have
- [x] Valid DESCRIPTION file
- [x] Appropriate license
- [x] Author and maintainer specified
- [ ] Package passes R CMD check with 0 errors, 0 warnings, 0 notes
- [ ] Examples run without error
- [ ] Help pages for all exported functions
- [ ] Valid NAMESPACE

### Must Not Have
- [ ] Non-standard directories (verify)
- [ ] Compiled code warnings
- [ ] Use of .Internal() (check)
- [ ] Absolute paths in code
- [ ] Writing to user's home directory
- [ ] Excessive startup messages

### Best Practices
- [ ] Package size reasonable (< 5MB)
- [ ] Fast examples (< 5 seconds)
- [ ] Cross-platform compatibility
- [ ] No external dependencies in examples
- [ ] Proper citation format
- [ ] Clean check on all platforms

## Success Criteria

- [ ] R CMD check --as-cran shows 0/0/0 (errors/warnings/notes)
- [ ] rhub checks pass on all test platforms
- [ ] winbuilder checks pass (devel, release, oldrelease)
- [ ] Package size < 5MB
- [ ] All URLs accessible
- [ ] LICENSE compliant
- [ ] Ready for CRAN submission

## Tools

```r
# Local checks
devtools::check(cran = TRUE)

# Remote checks
rhub::check_for_cran()
devtools::check_win_devel()
devtools::check_win_release()
devtools::check_win_oldrelease()

# Additional checks
goodpractice::gp()
spelling::spell_check_package()
```

## Common CRAN Issues to Avoid

1. Examples running too long
2. Writing to user's home directory
3. Unquoted package names in DESCRIPTION
4. Non-ASCII characters without declaration
5. Non-standard file/directory names
6. Broken URLs
7. Missing citation information
8. Compiler warnings

## References

- CRAN Repository Policy: https://cran.r-project.org/web/packages/policies.html
- Writing R Extensions: https://cran.r-project.org/doc/manuals/r-release/R-exts.html
- CRAN Submission Checklist: https://r-pkgs.org/release.html

## Notes

Budget extra time for addressing unexpected CRAN check results. First submission often reveals issues not caught by local checks.
