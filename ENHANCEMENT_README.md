# 📋 Enhancement Plan Documentation

This directory contains comprehensive planning and tracking documents
for bringing the sobol R package to release-quality standards suitable
for CRAN submission.

## 🚀 Quick Start

**New to the enhancement plan?** Start here: 1. Read
[ENHANCEMENT_OVERVIEW.md](https://alrobles.github.io/sobol/ENHANCEMENT_OVERVIEW.md)
for a quick reference guide 2. Review
[ENHANCEMENT_PLAN.md](https://alrobles.github.io/sobol/ENHANCEMENT_PLAN.md)
for the strategic overview 3. Use
[RELEASE_CHECKLIST.md](https://alrobles.github.io/sobol/RELEASE_CHECKLIST.md)
to track daily progress

## 📁 Document Structure

    sobol/
    ├── ENHANCEMENT_OVERVIEW.md      ← Start here! Quick reference guide
    ├── ENHANCEMENT_PLAN.md           ← Strategic plan and roadmap
    ├── RELEASE_CHECKLIST.md          ← Detailed task checklist
    └── .github/
        ├── SUBISSUE_1_CORE_CODE.md      ← Code verification tasks
        ├── SUBISSUE_2_DOCUMENTATION.md  ← Documentation tasks
        ├── SUBISSUE_3_TESTS.md          ← Testing tasks
        ├── SUBISSUE_4_METADATA.md       ← CRAN compliance tasks
        ├── SUBISSUE_5_VIGNETTES.md      ← Vignette enhancement tasks
        └── SUBISSUE_6_BENCHMARKS.md     ← Benchmarking tasks

## 📖 Document Guide

### For Project Managers

- **ENHANCEMENT_OVERVIEW.md** - High-level status and next actions
- **ENHANCEMENT_PLAN.md** - Strategic planning and timelines
- Progress tracking via RELEASE_CHECKLIST.md

### For Developers

- **Specific sub-issue files** in `.github/` for detailed tasks
- **RELEASE_CHECKLIST.md** for tracking completion
- Code quality tools listed in ENHANCEMENT_OVERVIEW.md

### For Reviewers

- **ENHANCEMENT_PLAN.md** - Quality gates and success criteria
- **RELEASE_CHECKLIST.md** - What’s been completed
- Sub-issue files for detailed task context

## 🎯 Current Status

**Overall Progress:** ~70% to release-ready

| Area            | Priority | Status                   |
|-----------------|----------|--------------------------|
| Documentation   | HIGH     | 🟡 Needs work (60%)      |
| CRAN Compliance | HIGH     | 🟡 Final review (75%)    |
| Tests           | MEDIUM   | 🟡 Reorganization (80%)  |
| Vignettes       | MEDIUM   | 🟡 Enhancement (70%)     |
| Benchmarks      | MEDIUM   | 🟠 Docs needed (50%)     |
| Core Code       | LOW      | ✅ Mostly complete (90%) |

## 🎯 High-Priority Actions

### This Week

1.  Fix typo in README: “Test valiting” → “Test validating”
2.  Run `R CMD check --as-cran` and document results
3.  Add cross-references in man pages
4.  Run spell checker

### Next Two Weeks (CRAN Critical)

1.  Complete DESCRIPTION review
2.  Run rhub checks
3.  Create NEWS.md
4.  Address all R CMD check NOTEs

## 📚 Key Documents Explained

### ENHANCEMENT_OVERVIEW.md

Quick reference guide linking to all planning materials. Use this to
navigate the enhancement plan and find what you need quickly.

**Best for:** Getting oriented, finding documents, checking status

### ENHANCEMENT_PLAN.md

Comprehensive strategic document covering: - Current state assessment -
Six enhancement goals - Three-phase implementation roadmap - Quality
gates and success criteria - Risk assessment

**Best for:** Understanding strategy, planning work, reviewing quality
standards

### RELEASE_CHECKLIST.md

Detailed checklist with every task needed for release. Organized by the
six sub-issues with checkboxes for tracking progress.

**Best for:** Daily work, task tracking, progress monitoring

### Sub-Issue Files (.github/)

Individual tracking documents for each of the six workstreams: 1. Core
Code Verification 2. Documentation Cleanup 3. Test Suite Organization 4.
Metadata & CRAN Compliance 5. Vignettes & Examples 6. Benchmarking &
Performance

**Best for:** Deep dive into specific workstreams, understanding task
context

## 🛠️ Useful Commands

### Quick Checks

``` r
# CRAN check
devtools::check(cran = TRUE)

# Spell check
spelling::spell_check_package()

# Style check
lintr::lint_package()

# Coverage
covr::package_coverage()
```

### Pre-Submission

``` r
# Remote CRAN checks
rhub::check_for_cran()
devtools::check_win_devel()

# Best practices
goodpractice::gp()
```

## 📊 Tracking Progress

Update RELEASE_CHECKLIST.md as tasks are completed: - \[ \] Incomplete
task - \[x\] Completed task

Track overall metrics: - R CMD check status - Test coverage percentage -
Package size - Documentation completeness

## 🤝 Contribution Workflow

1.  **Choose a task** from RELEASE_CHECKLIST.md
2.  **Review context** in relevant sub-issue file
3.  **Make changes** following the guidelines
4.  **Test thoroughly** using commands above
5.  **Update checklists** to track completion
6.  **Commit and push** changes

## 📞 Help and Resources

### Documentation

- [R Packages Book](https://r-pkgs.org/)
- [CRAN Policy](https://cran.r-project.org/web/packages/policies.html)
- [Writing R
  Extensions](https://cran.r-project.org/doc/manuals/r-release/R-exts.html)

### Tools

- [devtools](https://devtools.r-lib.org/)
- [rhub](https://r-hub.github.io/rhub/)
- [goodpractice](https://github.com/MangoTheCat/goodpractice)

### Style Guides

- [tidyverse style](https://style.tidyverse.org/)
- [roxygen2 docs](https://roxygen2.r-lib.org/)

## 🎓 Next Steps

1.  **Familiarize yourself** with the document structure
2.  **Review current status** in ENHANCEMENT_OVERVIEW.md
3.  **Pick high-priority tasks** from RELEASE_CHECKLIST.md
4.  **Read relevant sub-issue** for context
5.  **Start working** on improvements

## ✅ Quality Gates

Before considering the package release-ready:

R CMD check: 0 errors, 0 warnings, 0 notes

Test coverage \> 90%

All functions documented

At least 2 comprehensive vignettes

Package size \< 5MB

All URLs accessible

NEWS.md updated

CITATION file created

------------------------------------------------------------------------

**Version:** 1.0 **Last Updated:** 2026-04-23 **Status:** Active
Planning Phase

For questions or updates, refer to individual planning documents or
create a GitHub issue.
