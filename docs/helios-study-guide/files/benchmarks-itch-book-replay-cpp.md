# File Guide: `benchmarks/itch_book_replay.cpp`

## 1. File purpose

Maps a historical ITCH BinaryFILE, attempts to prefault it, reconstructs one target symbol, times load and process phases, and prints counters/final book summary.

## 2. Architecture position

End-to-end executable joining OS ingestion, parser, replay adapter, and order book; it supplies the most realistic tracked workload.

## 3. Dependencies

Replay/book/parser headers, POSIX file/stat/mmap/advice APIs, stdio/conversion, and `steady_clock`.

## 4. Symbols defined

Fallback macro `MAP_POPULATE` and `main`.

## 5. Symbols consumed

All ingestion syscalls, parser callback, `BookReplay`, `OrderBook` queries/counters.

## 6. Source-order walkthrough

- **Lines 1–5:** comments claim prefault behavior and give a manual profiling command; source/build provenance is separate from CMake.
- **Lines 6–23:** duplicate chrono include and namespace directive; macOS fallback turns `MAP_POPULATE` into zero, so the same output label can hide different behavior.
- **Lines 25–32:** validates two arguments and `open`, but ignores `fstat` failure and does not close fd if later mapping fails.
- **Lines 34–42:** starts load timer, maps file with private read and optional populate, checks `MAP_FAILED`.
- **Lines 44–52:** issues two unchecked `madvise` calls and touches every 4096th byte. The stride is hard-coded rather than queried, last partial page is naturally touched only if its first offset appears, and volatile sum prevents elimination.
- **Lines 54–60:** `t1` ends load before constructing a large book. Therefore “process (parse+book)” includes `OrderBook` construction and hash/pool/ladders, not only parse/mutations.
- **Lines 62–67:** millisecond truncation can make short process time zero.
- **Lines 69–76:** throughput divides modeled parser callback count by the entire process phase, which also includes nonmodeled frame scan and book construction. Adapter counters describe only subsets.
- **Lines 77–89:** displays level counts and cents-scaled prices, inheriting HEL-001. Crossed test labels a locked book as “YES (bug/locked)” even though locked need not itself be corruption. Empty-side diagnosis is speculative.
- **Lines 91–93:** releases mapping and descriptor without checking.

## 7. Inputs and outputs

Path and symbol arguments; output timings, rates, counters, and final top-level state.

## 8. Ownership and lifetime

Mapping owns visible file address range until `munmap`; parser borrows it. `OrderBook` outlives `BookReplay` due declaration order and is destroyed before mapping cleanup at scope end only after explicit cleanup lines.

## 9. Invariants

Valid file resources; target is at most eight semantic bytes; book range/unit covers feed; callback is synchronous; denominators and phase names match included work; mutations preserve core invariants.

## 10. Errors and edge cases

Stat/advice failure, empty/too-large file, missing residency, symbol truncation, valid price above ceiling, malformed frames, duplicate IDs, zero shares, millisecond zero, resource leaks on early return.

## 11. Performance implications

Load mode targets warm-page replay. Process includes eager construction of one million `PriceLevel` elements across sides (for 500k range), parser scan, symbol checks, hash/pool activity, and counters. Results are not pure book latency.

## 12. Technical debt

Price precision/range, checked OS preparation, page-size query, RAII resources, outcome taxonomy, phase definition, denominator, manifest/raw data, output semantics.

## 13. Related findings

HEL-001–007, HEL-012, HEL-017, HEL-022–026, HEL-033, HEL-035–038, HEL-042–044, HEL-048.

## 14. Interview questions

- What work is included in `proc_ms`?
- Does MAP_POPULATE guarantee fault-free replay on every platform?
- Why is `n/proc_ms` not accepted book mutations per second?
- How much memory does construction provision?

## 15. Exercises

Draw a phase timeline with every syscall/allocation. Define five simultaneous throughput denominators. Calculate how the $5,000 and cent-scale assumptions interact.
