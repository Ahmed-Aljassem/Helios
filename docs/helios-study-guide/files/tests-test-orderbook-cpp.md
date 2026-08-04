# File Guide: `tests/test_orderbook.cpp`

## 1. File purpose

The only registered GoogleTest source; combines basic behavior checks, randomized stress, performance printouts, and a custom test `main`.

## 2. Architecture position

Black-box client of the public `OrderBook` API. It is currently the entire automated CTest verification surface.

## 3. Dependencies

Order book, GoogleTest, RNG/vectors/clocks/iostream/formatting/sorting.

## 4. Symbols defined

Fixtures `OrderBookTest`, `OrderBookStressTest`; helper `printTiming`; 12 tests; test-program `main`.

## 5. Symbols consumed

All principal book mutations/queries and price conversion helpers.

## 6. Source-order walkthrough

### Lines 14–77 — basic cases

The fixture default-constructs a fresh book. `AddBidOrder` checks best/count/lookup; `AddAskOrder` omits ID lookup; `BestBidAsk` checks extrema/spread; `CancelOrder` checks empty sentinel and lookup removal; full/partial market tests check total fill and next/empty best. `FIFOOrdering` cancels the middle ID and proves only identity removal—not FIFO execution order—because head order is never observed.

### Lines 81–99 — stress fixture

Another fresh default book and a reporting helper. `printTiming` divides by seconds/ops without zero guards and mixes benchmarking output into correctness tests.

### Lines 101–129 — 100k Adds

Fixed RNG generates valid range values; timing includes RNG and all Adds, not setup. Assertions verify total count and merely that both sides have at least one level, not aggregates/reachability.

### Lines 131–183 — mixed Add/Cancel

Despite the “50/50” heading/comment conflict, logic is roughly 70% Add/30% Cancel when possible. RNG, temporary distribution construction, vector bookkeeping, and book calls are timed. Final count equals driver adds minus successful cancels, which shares driver state but is a useful coarse check.

### Lines 185–225 — market execution

Builds 50k orders, times 1,000 random sweeps, and prints state. It has no assertions; any non-crashing output passes.

### Lines 227–274 — chrono latency distribution

Warm-up grows state, then each Sell Add is bracketed by `high_resolution_clock`; timer calls and book growth are included. It prints empirical values but asserts nothing and records no provenance.

### Lines 276–308 — million Adds

Times RNG plus Adds and prints progress inside the timed interval every 200k operations, severely contaminating throughput. Final count is the sole assertion.

### Lines 310–361 — claimed invariant stress

Generates Add/Cancel/market operations. Market sweeps remove IDs without updating `active_orders`, so later cancels may target stale IDs (HEL-034). The first “invariant” compares `getTotalOrders()` with itself through both ternary branches and can never distinguish corruption (HEL-009). Best-price checks are shallow. The final log “no invariant violations” overstates what was checked.

### Lines 363–366 — main

Initializes and runs GoogleTest; CMake registers this executable once.

## 7. Inputs and outputs

GTest flags and deterministic RNG seeds enter. Pass/fail status plus extensive timing/log output leave.

## 8. Ownership and lifetime

Each fixture owns a book for one test. Vectors own IDs/samples. `getOrder` pointers are used only immediately in basic tests.

## 9. Invariants

Desired: map/queue bijection, FIFO, aggregates, bitmaps, best caches, global counts, pool liveness. Actual tests directly observe only a small public subset and lack independent traversal/oracle.

## 10. Errors and edge cases

No duplicate ID, zero qty, range endpoints/invalid range, missing cancel, modify, partial same-level FIFO execution, map growth failure, pool lifetime, malformed protocol, or replay behavior. Timing divisions can fail on zero duration.

## 11. Performance implications

Stress outputs are non-controlled whole-test timings. RNG, vector operations, progress I/O, assertions, debug/release configuration, and growing state prevent use as credible latency evidence.

## 12. Technical debt

False FIFO confidence, tautological invariant, stale generator IDs, no assertions in performance cases, mixing tests/benchmarks, missing subsystem coverage/reference model.

## 13. Related findings

HEL-004–005, HEL-009, HEL-011, HEL-028–029, HEL-033–035, HEL-043, HEL-046–048.

## 14. Interview questions

- What mutant implementation would `FIFOOrdering` fail to catch?
- Why is the invariant assertion tautological?
- Which timed tests include RNG or I/O?
- How would an oracle remain independent?

## 15. Exercises

For every test, write one incorrect implementation that still passes. Redesign the assertion set on paper so every operation compares a reference model and full canonical state.
