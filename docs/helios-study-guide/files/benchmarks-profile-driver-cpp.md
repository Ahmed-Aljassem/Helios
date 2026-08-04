# File Guide: `benchmarks/profile_driver.cpp`

## 1. File purpose

Creates a long mixed Add/Cancel workload for sampling profilers such as `perf`, keeping a roughly bounded driver-managed live set.

## 2. Architecture position

Whole-program profiling workload, excluded from current CMake targets.

## 3. Dependencies

Order book, random/vector/stdio/conversion headers, and Linux affinity.

## 4. Symbols defined

`pin` and `main`.

## 5. Symbols consumed

Book Add/Cancel/active-level query, RNG/distributions, affinity API.

## 6. Source-order walkthrough

- **Lines 1–4:** documents a manual compiler command; paths assume a working directory/layout and omit `price_level.cpp` even though the library needs its definitions, weakening reproducibility.
- **Lines 13–16:** Linux affinity is unconditional in source and unchecked.
- **Lines 18–25:** parses operation/core arguments weakly; constructs 1..50000 book with large pool; initializes deterministic RNG and price/qty/side/action distributions.
- **Lines 27–28:** seeds every active price as BUY, affecting initial side balance and adding 2,001 orders not recorded in `live`.
- **Lines 30–35:** CAP aims to bound only IDs in the driver vector, not seeded orders; reserve allows twice CAP.
- **Lines 36–49:** each iteration calls RNG to choose cancel/add, random cancellation index, side/price/qty, mutates vector by swap-pop, and reads active bid levels into a sink. These driver operations are intentionally inside the sampled program and can dominate profiles.
- **Lines 50–52:** emits counts to stderr so work/result is observable.

## 7. Inputs and outputs

Arguments control iteration count and core. Output is aggregate operation/live/sink counts; profiler produces external samples.

## 8. Ownership and lifetime

`main` owns book, RNG, and live-ID vector for process lifetime. Vector IDs are values, not pointers.

## 9. Invariants

Every `live` ID should name a live book order; seeded IDs are deliberately absent. Successful cancel removes exactly one driver ID. CAP is a soft threshold because random action can add below/near it.

## 10. Errors and edge cases

Negative/huge N, invalid core, affinity failure, allocation failure, driver/book state divergence, auto-ID overflow, platform incompatibility.

## 11. Performance implications

Sampling includes RNG modulo/distributions, vector accesses, branch choice, bookkeeping, query, and book. It is useful end-to-end driver cost but not isolated book cost. Random cancels stress hash/order locality.

## 12. Technical debt

Not built, manual command questionable, no phase markers, RNG contamination, unchecked environment, weak workload manifest.

## 13. Related findings

HEL-005, HEL-028, HEL-030, HEL-032–035, HEL-045–048.

## 14. Interview questions

- Why pre-generate a profile trace, and how could that itself change cache behavior?
- Is the live count truly bounded by CAP?
- What do seeded untracked orders do to the workload?

## 15. Exercises

Classify each line in the loop as subject, workload generation, bookkeeping, or observation. Design separate whole-program and book-only profiles with the same logical trace.
