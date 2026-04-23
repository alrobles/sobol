# Sub-Issue 5: Add Vignettes and User-Facing Usage Examples

**Parent Issue:** Enhancement Plan - Prepare sobol R package for release-quality standards
**Priority:** Medium
**Status:** Vignettes Exist, Enhancement Opportunities
**Assignee:** TBD
**Estimated Effort:** 12-20 hours

## Objective

Enhance existing vignettes and add user-facing usage examples to improve package usability and adoption.

## Background

Two vignettes already exist (`sobol.Rmd` and `sobol-sequences.Rmd`). This task focuses on enhancing them and adding practical examples.

## Tasks

### Existing Vignette Review
- [ ] Verify all code chunks execute without errors
- [ ] Check vignette build time (< 60 seconds)
- [ ] Review plots and add figure captions
- [ ] Check for typos and grammar
- [ ] Ensure examples are reproducible
- [ ] Add timing information for code examples
- [ ] Cross-reference between vignettes

### sobol.Rmd Enhancement
- [ ] Add Quick Start section (< 5 minutes)
- [ ] Expand parallel workflows with concrete example
- [ ] Add troubleshooting section
- [ ] Include performance tips
- [ ] Show integration with optimization packages
- [ ] Add comparison with random sampling visualization
- [ ] Include memory usage tips

### sobol-sequences.Rmd Enhancement
- [ ] Add historical context
- [ ] Include mathematical background (accessible)
- [ ] Add convergence comparison plots
- [ ] Show high-dimensional projection quality
- [ ] Include practical QMC integration example
- [ ] Add references to key papers

### New Vignette Ideas
- [ ] "Quick Start Guide" (5-minute introduction)
- [ ] "Performance Benchmarks" (comprehensive comparisons)
- [ ] "Sensitivity Analysis with Sobol Sequences"
- [ ] "Advanced Topics" (parallel processing, state management)
- [ ] "Comparison Guide" (vs. LHS, random, other QMC)

### Usage Examples Enhancement
- [ ] Review `inst/examples/usage_examples.R`
- [ ] Add Monte Carlo integration examples
- [ ] Add optimization examples
- [ ] Add sensitivity analysis example
- [ ] Show error handling patterns
- [ ] Add parallel processing example
- [ ] Include visualization examples

### Vignette Best Practices
- [ ] Keep build time under 60 seconds
- [ ] Use small datasets for examples
- [ ] Cache expensive computations if needed
- [ ] Add "Further Reading" sections
- [ ] Include links to function documentation
- [ ] Add "Next Steps" at end of each vignette
- [ ] Use consistent notation and terminology

## Vignette Topics to Cover

### Must Cover
- [x] Basic usage (covered in sobol.Rmd)
- [x] Incremental generation (covered)
- [x] Skip functionality (covered)
- [x] Batch generation (covered)
- [ ] Performance characteristics (partial)
- [ ] Best practices (partial)
- [ ] Common pitfalls

### Should Cover
- [ ] Parallel workflows (needs expansion)
- [ ] Integration with optimization
- [ ] Comparison with alternatives
- [ ] Memory management
- [ ] Reproducibility guarantees

### Nice to Have
- [ ] Sensitivity analysis workflow
- [ ] QMC theory background
- [ ] Advanced state management
- [ ] Custom scaling functions
- [ ] Visualization techniques

## Example Categories

### Basic Examples
- [x] Simple point generation
- [x] Parameter space exploration
- [ ] Function optimization
- [ ] Monte Carlo integration
- [ ] Reproducibility demonstration

### Advanced Examples
- [ ] Parallel worker coordination
- [ ] Adaptive sampling
- [ ] State save/restore
- [ ] Integration with popular packages
- [ ] Custom workflows

### Domain-Specific Examples
- [ ] Machine learning hyperparameter tuning
- [ ] Sensitivity analysis
- [ ] Financial modeling
- [ ] Scientific simulation
- [ ] A/B testing

## Success Criteria

- [ ] All vignettes build without errors
- [ ] Build time < 60 seconds per vignette
- [ ] Examples are clear and practical
- [ ] Coverage of common use cases
- [ ] Links to function documentation work
- [ ] Figures have descriptive captions
- [ ] Cross-references between vignettes
- [ ] Code follows best practices

## Dependencies

- Sub-Issue 2 (Documentation) - for cross-references
- Sub-Issue 6 (Benchmarks) - for performance vignette

## Files to Work On

```
vignettes/
├── sobol.Rmd                  # Getting Started guide
├── sobol-sequences.Rmd        # Technical introduction
└── [new vignettes]

inst/
└── examples/
    └── usage_examples.R       # Example code collection
```

## Tools

```r
# Build vignettes
devtools::build_vignettes()

# Preview vignette
devtools::build_rmarkdown("vignettes/sobol.Rmd")

# Check build time
system.time(rmarkdown::render("vignettes/sobol.Rmd"))
```

## Guidelines

1. **Keep it practical** - Show real-world use cases
2. **Keep it simple** - Start basic, build complexity gradually
3. **Keep it fast** - Vignettes should build quickly
4. **Keep it visual** - Use plots to illustrate concepts
5. **Keep it linked** - Cross-reference related topics

## References

- R Packages book: https://r-pkgs.org/vignettes.html
- rmarkdown: https://rmarkdown.rstudio.com/
- knitr: https://yihui.org/knitr/

## Notes

Consider creating an "Articles" section on the pkgdown site separate from vignettes for longer-form content that doesn't need to ship with the package.
