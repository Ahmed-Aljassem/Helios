# File Guide: `benchmarks/benchmark_orderbook.cpp`

## 1. File purpose

Measures add-only batch throughput and per-operation cycle-derived latency under generated inputs.

## 2. Architecture position

Primary microbenchmark client of `OrderBook` and `rdtsc_timer.hpp`; not a test or replay workload.

## 3. Dependencies

Book/timer headers, Linux affinity API, C stdio, vectors, Mersenne Twister/distributions, sorting, thread hardware count.

## 4. Symbols defined

`pinToCore`, `Input`, `makeInputs`, `batchThroughputNs`, `perOpLatenciesNs`, `median`, `main`.

## 5. Symbols consumed

`OrderBook::addOrder`, timer functions/class, optimizer barrier, OS affinity.

## 6. Source-order walkthrough

- **Lines 1–15:** comments enumerate methodological claims; several are conditional or overstated—pin failure is ignored, overhead subtraction is not proven additive, and median-of-run statistics need exact labels.
- **Lines 30–41:** Linux-only affinity ignores return status; other platforms no-op while later output still says pinned.
- **Lines 43–61:** `Input` carries side/price/qty. Fixed-seed generation occurs before timing with uniform price 9000–11000, quantity 100–1000, side 50/50.
- **Lines 63–77:** batch creates a large pre-sized book and seeds every price with a bid, then times only the loop of 200k Adds plus optimizer barrier. Construction/generation/warm-up/destruction are excluded. State grows and map nodes still allocate.
- **Lines 79–109:** per-op creates/seeds another book, adds 20k discarded warm-up orders, then brackets every Add. It subtracts minimum empty-timer ticks with floor at zero, sorts, and converts. Timer overhead and operation can interact.
- **Lines 111–115:** median copies/sorts and selects upper middle for even count.
- **Lines 117–133:** core defaults to last reported hardware thread; unchecked `atoi`; claims pin; calibrates; generates fixed inputs.
- **Lines 135–163:** reports batch mean, then seven independent book runs over identical input order. It takes median of each run's percentile. `median(maxes)` is not the worst single sample despite the label.

## 7. Inputs and outputs

Optional core argument; deterministic generated workload. Output is formatted calibration, batch mean/rate, and aggregated quantiles/max statistic.

## 8. Ownership and lifetime

Each function owns a fresh book/input/result vectors. All book state is destroyed after a run; returned latency vector owns converted samples but raw ticks/provenance are not persisted.

## 9. Invariants

Inputs must be nonempty and in-range; every reported operation must succeed; timer/platform/affinity conditions must hold; statistic labels must match computation. Success IDs are not explicitly asserted.

## 10. Errors and edge cases

Zero hardware count, invalid/out-of-range core, empty input division/indexing, allocation failure, non-x86 zero clock, unsuccessful Adds, migration, overflow, system noise.

## 11. Performance implications

Measures add with growing live state, warmed occupied levels, huge pool reserve, likely map node allocation, random prices/sides, and per-op fence perturbation. It says nothing direct about cancel, modify, execute, replay, steady live population, or worst-case bounds.

## 12. Technical debt

Unchecked environment, questionable subtraction, mislabeled max, absent raw data/manifest, add-only bias, library flag mismatch, no capacity-event stratification.

## 13. Related findings

HEL-005–006, HEL-028–031, HEL-033, HEL-035, HEL-040, HEL-043, HEL-048.

## 14. Interview questions

- What exact instructions are inside the timed boundary?
- Why does seeding each level not eliminate map allocations?
- Why is median of maxima not worst case?
- What alternative does batch timing answer?

## 15. Exercises

Create a measurement-contract table for batch and per-op paths. Plot conceptually latency versus live-state size. Specify raw columns and manifest fields for a credible rerun.
