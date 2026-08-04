# File Guide: `tests/manual_test.cpp`

## 1. File purpose

Interactive demonstration/stress executable with five scenarios and human-readable timing/state output.

## 2. Architecture position

Manual exploratory client, built but not registered with CTest. It is neither an assertion-based test nor controlled benchmark.

## 3. Dependencies

Order book and standard I/O, formatting, RNG, chrono, vector, string facilities.

## 4. Symbols defined

Formatting helpers, `Timer`, demos 1–5, and `main`.

## 5. Symbols consumed

Book operations/queries and price formatting/conversion.

## 6. Source-order walkthrough

- **Lines 11–30:** presentation helpers print headings and comma-group positive `size_t` values.
- **Lines 32–45:** `Timer` captures `high_resolution_clock` at construction and returns elapsed wall duration; clock monotonicity is implementation-dependent.
- **Lines 47–68, Demo 1:** adds a six-order book, calls no-op `printBook`, sweeps 800 asks, and prints only fill count. Visualization promise is unfulfilled (HEL-039).
- **Lines 70–111, Demo 2:** generates 10k Gaussian dollar doubles, truncates through `dollarToPrice`, times RNG plus Adds, and prints state. “Realistic” describes only a toy distribution.
- **Lines 113–190, Demo 3:** times one million mixed operations including RNG and progress I/O. Market execution removes book orders but their IDs remain in `active_orders`, causing stale cancellation attempts (HEL-034). Reported average is whole-driver time.
- **Lines 192–237, Demo 4:** adds 100k same-price bids, then cancels every other ID, useful for middle unlink stress but without structural/FIFO assertions.
- **Lines 239–278, Demo 5:** creates 1,000 ask levels × 100 orders, then fully sweeps. Reports per-share, a denominator that hides per-order/level costs; no assertion verifies complete state.
- **Lines 280–305:** optional integer chooses one demo; unknown choices run none but still print completion.

## 7. Inputs and outputs

Optional demo number; deterministic generated operations; console narrative/timings.

## 8. Ownership and lifetime

Each demo owns and destroys its book. ID vectors own numeric IDs only. Timers are stack-local.

## 9. Invariants

The demos assume successful in-range Adds and consistent driver IDs but do not assert them. Demo 3 violates its own live-ID bookkeeping after sweeps.

## 10. Errors and edge cases

Invalid demo text, floating conversion boundaries, zero elapsed/fill division, stale IDs, allocation failure, empty side, no-op visualization.

## 11. Performance implications

All timings include driver work; progress output heavily perturbs Demo 3. Workloads teach shapes—same-level burst and deep sweep—but results are not isolated latency claims.

## 12. Technical debt

No assertions, stale bookkeeping, overbroad “realistic/ultra-low-latency” labels, no-op print dependency, and benchmark/test role ambiguity.

## 13. Related findings

HEL-019, HEL-028–029, HEL-033–034, HEL-039, HEL-042–043, HEL-048.

## 14. Interview questions

- Which demo best stresses pointer chasing?
- Why is per-share sweep time a weak universal metric?
- How can manual demos complement but not replace tests?

## 15. Exercises

Predict exact final state for a ten-order miniature of each demo. List every line inside each timed boundary and classify contamination.
