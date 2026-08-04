# File Guide: `include/rdtsc_timer.hpp`

## 1. File purpose

Supplies x86 timestamp-counter reads, optimizer barriers, calibration, conversion, timer-overhead sampling, and a percentile helper.

## 2. Architecture position

Benchmark instrumentation, not production order-book logic. Read with [08](../08-benchmarking-and-measurement.md).

## 3. Dependencies

Integer/time/vector/algorithm headers, x86 intrinsics when selected, compiler inline assembly, POSIX `clock_gettime`.

## 4. Symbols defined

Macro `ORDERBOOK_HAS_RDTSC`; `tscStart`, `tscEnd`, `doNotOptimize`, `nsBetween`, `TscClock`, `measureTimerOverhead`, `percentile`.

## 5. Symbols consumed

Primary benchmark and any timing client consume these utilities.

## 6. Source-order walkthrough

- **Lines 1–10:** comments correctly distinguish invariant TSC from core cycles but overpromise platform conditions and call non-x86 zero a “clock.”
- **Lines 16–21:** compile-time architecture detection does not prove runtime RDTSCP/invariant-TSC availability.
- **Lines 25–44:** start uses LFENCE/RDTSC/LFENCE; end uses RDTSCP/LFENCE. `aux` is discarded, so migration detection is lost. Ordering claims should follow official instruction semantics.
- **Lines 45–48:** unsupported architectures return zero, yielding valid-looking invalid samples (HEL-040).
- **Lines 50–55:** compiler barrier makes value observable to optimizer; GCC-style asm is not portable C++.
- **Lines 57–61:** nanosecond difference assumes `b >= a`; separate unsigned casts can misbehave for subsecond borrowing unless sec/nsec relation is normalized by monotonic ordering (the algebra happens to combine modulo only within range assumptions).
- **Lines 63–93:** constructor calibrates once. It busy-waits ~200 ms, brackets clock reads asymmetrically, assumes no migration and divides by nonzero elapsed time. Fallback reports 1 GHz despite zero reads.
- **Lines 95–107:** chooses minimum empty-bracket delta, then callers subtract it. Minimum characterizes a lower instrumentation path, not an independently additive constant.
- **Lines 109–115:** expects sorted input; index is floor of `N*p`, an explicitly chosen but undocumented empirical quantile convention.

## 7. Inputs and outputs

Hardware/OS time enters; raw ticks, calibrated Hz, converted ns, overhead estimate, and percentile value leave.

## 8. Ownership and lifetime

No dynamic ownership except caller-provided vectors. `TscClock` stores one calibration scalar for its lifetime.

## 9. Invariants

End sample must correspond to same valid counter domain and ordered interval as start; calibration denominator and Hz must be positive; percentile input sorted; unsupported samples never treated as real.

## 10. Errors and edge cases

Runtime feature absence, virtualization, core migration, signal/preemption, counter wrap, signed/narrowing clients, failed clock calls, non-x86, empty samples, p outside [0,1].

## 11. Performance implications

Fences and counter reads materially perturb short operations. Per-op timing serializes execution and can change cache/pipeline behavior. Calibration burns one core briefly.

## 12. Technical debt

Capability/AUX checks, affinity integration, error results, portable fallback, quantile definition, raw-sample discipline, and claims around subtraction.

## 13. Related findings

HEL-020, HEL-029–031, HEL-033, HEL-040.

## 14. Interview questions

- What ordering does each fence/instruction provide?
- Why not subtract minimum overhead blindly?
- What does invariant TSC not guarantee?
- How would you detect migration?

## 15. Exercises

Draw the instruction timeline for start/op/end. Derive batch timing as an alternative. Define validation metadata required before converting a sample to nanoseconds.
