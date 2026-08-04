# Tracked File Coverage and Evidence Catalog

**Repository evidence:** `git ls-files` at the audited revision reports 48 paths. Every path is mapped below. The 23 code/header/test/benchmark/build/script files have dedicated walkthroughs under [`files/`](files/README.md). The 25 documentation/evidence paths are interpreted here and cross-linked to shared chapters. Opaque Git internals, caches, and untracked binaries are intentionally outside scope.

## Complete inventory

| # | Tracked path | Coverage |
|---:|---|---|
| 1 | `.gitignore` | [Dedicated guide](files/config-gitignore.md) |
| 2 | `CMakeLists.txt` | [Dedicated guide](files/CMakeLists.md) |
| 3 | `README.md` | [README evidence note](#readmemd) |
| 4 | `benchmarks/benchmark_orderbook.cpp` | [Dedicated guide](files/benchmarks-benchmark-orderbook-cpp.md) |
| 5 | `benchmarks/itch_book_replay.cpp` | [Dedicated guide](files/benchmarks-itch-book-replay-cpp.md) |
| 6 | `benchmarks/itch_replay.cpp` | [Dedicated guide](files/benchmarks-itch-replay-cpp.md) |
| 7 | `benchmarks/profile_driver.cpp` | [Dedicated guide](files/benchmarks-profile-driver-cpp.md) |
| 8 | `docs/.gitignore` | [Evidence note](#docsgitignore) |
| 9 | `docs/ARCHITECTURE.md` | [Evidence note](#docsarchitecturemd) |
| 10 | `docs/aligned_results.txt` | [Evidence note](#test-output-artifacts) |
| 11 | `docs/basline_million_ops.txt` | [Evidence note](#manual-workload-artifacts) |
| 12 | `docs/bench_aligned.txt` | [Evidence note](#rdtsc-benchmark-artifacts) |
| 13 | `docs/bench_array.txt` | [Evidence note](#rdtsc-benchmark-artifacts) |
| 14 | `docs/bench_unaligned.txt` | [Evidence note](#rdtsc-benchmark-artifacts) |
| 15 | `docs/flamegraph.svg` | [Evidence note](#flamegraph-placeholders) |
| 16 | `docs/flamegraph_itch.svg` | [Evidence note](#flamegraph-placeholders) |
| 17 | `docs/flamegraph_itch_FULL.svg` | [Evidence note](#flamegraph-placeholders) |
| 18 | `docs/flamegraph_itch_prefaulted.svg` | [Evidence note](#flamegraph-placeholders) |
| 19 | `docs/flamegraph_itch_prefaulted_FULL.svg` | [Evidence note](#flamegraph-placeholders) |
| 20 | `docs/flamegraph_itch_uint64.svg` | [Evidence note](#flamegraph-placeholders) |
| 21 | `docs/flamegraph_itch_uint64_FULL.svg` | [Evidence note](#flamegraph-placeholders) |
| 22 | `docs/itch_replay_2026-06-09_003448.md` | [Evidence note](#replay-report-artifacts) |
| 23 | `docs/itch_replay_2026-06-09_003836.md` | [Evidence note](#replay-report-artifacts) |
| 24 | `docs/optimization_01_memory_pool.md` | [Evidence note](#optimization-journals) |
| 25 | `docs/optimization_02_array_ladder.md` | [Evidence note](#optimization-journals) |
| 26 | `docs/optimization_03_hashmap_attempt.md` | [Evidence note](#optimization-journals) |
| 27 | `docs/pooled_million_ops.txt` | [Evidence note](#manual-workload-artifacts) |
| 28 | `docs/pooled_results.txt` | [Evidence note](#test-output-artifacts) |
| 29 | `docs/profiling_NASDAQ_sample500MB.md` | [Evidence note](#profiling-notes) |
| 30 | `docs/profiling_notes.md` | [Evidence note](#profiling-notes) |
| 31 | `docs/profiling_real_NASDAQ.md` | [Evidence note](#profiling-notes) |
| 32 | `include/book_replay.hpp` | [Dedicated guide](files/include-book-replay-hpp.md) |
| 33 | `include/itch_parser.hpp` | [Dedicated guide](files/include-itch-parser-hpp.md) |
| 34 | `include/object_pool.hpp` | [Dedicated guide](files/include-object-pool-hpp.md) |
| 35 | `include/order.hpp` | [Dedicated guide](files/include-order-hpp.md) |
| 36 | `include/orderbook.hpp` | [Dedicated guide](files/include-orderbook-hpp.md) |
| 37 | `include/price_level.hpp` | [Dedicated guide](files/include-price-level-hpp.md) |
| 38 | `include/rdtsc_timer.hpp` | [Dedicated guide](files/include-rdtsc-timer-hpp.md) |
| 39 | `include/types.hpp` | [Dedicated guide](files/include-types-hpp.md) |
| 40 | `run_itch_replays.sh` | [Dedicated guide](files/run-itch-replays-sh.md) |
| 41 | `src/order.cpp` | [Dedicated guide](files/src-order-cpp.md) |
| 42 | `src/orderbook.cpp` | [Dedicated guide](files/src-orderbook-cpp.md) |
| 43 | `src/price_level.cpp` | [Dedicated guide](files/src-price-level-cpp.md) |
| 44 | `tests/alloc_check.cpp` | [Dedicated guide](files/tests-alloc-check-cpp.md) |
| 45 | `tests/manual_test.cpp` | [Dedicated guide](files/tests-manual-test-cpp.md) |
| 46 | `tests/test_order.cpp` | [Dedicated guide](files/tests-test-order-cpp.md) |
| 47 | `tests/test_orderbook.cpp` | [Dedicated guide](files/tests-test-orderbook-cpp.md) |
| 48 | `tests/test_price_level.cpp` | [Dedicated guide](files/tests-test-price-level-cpp.md) |

## Tracked documentation and artifact interpretation

### `README.md`

**Purpose and connection:** The public project narrative combines scope, architecture, performance snapshot, optimization history, build/run instructions, roadmap, and lessons. It points readers to all main code paths and historical documents.

**What it means:** Its architecture description is a useful intent statement, not the executable specification. The source shows a direct ladder today, while historical/stale notes still describe `std::map`. Its C++20 requirement conflicts with CMake's enforced C++17 (HEL-010). The stated test surface omits the lack of parser/replay/pool verification. Performance numbers are historical claims requiring their original binary/environment manifests.

**How to study it:** Read once before [02](02-complete-architecture.md), then again after [12](12-technical-debt-and-limitations.md). On the second pass label each paragraph repository evidence, historical claim, inference, or roadmap. Do not quote the 26.6 ns result without [08's](08-benchmarking-and-measurement.md) qualifications.

**Interview lesson:** A README is an argument about a system. Every strong noun—“ultra-low-latency,” “O(1),” “allocation-free,” “realistic,” “worst”—creates a proof obligation.

### `docs/.gitignore`

The file is empty. It currently changes no Git classification inside `docs/`. Its likely purpose was to establish a location for documentation-specific generated-artifact rules, but repository evidence provides no active policy. This connects to HEL-016/048: evidence retention is governed mostly by the root ignore file and manual choice.

### `docs/ARCHITECTURE.md`

**Purpose:** Short early architecture/design note covering fixed-point prices, price-level/map choices, complexities, layout, and future optimizations.

**Historical value:** It documents the pre-direct-ladder design: it names `std::map` for price levels. Current `OrderBook` uses vectors plus bitmaps, so this is historical, not current architecture. Its complexity table should be interpreted for that prior representation. The memory-layout and future-optimization ideas show reasoning evolution.

**Technical debt:** No version/date/commit banner distinguishes the historical state. See HEL-014 and compare [11](11-engineering-decisions-and-tradeoffs.md).

### Optimization journals

#### `docs/optimization_01_memory_pool.md`

Records before/after add-latency summaries attributed to replacing general per-order allocation with a pool and offers a mechanism: reused stable slots reduce allocator work. The current pool source supports the mechanism only conditionally; hash nodes still allocate and pool growth remains. The claimed residual maximum is not causally attributable without raw samples/build manifest. Study with HEL-006/013/021/035/044.

#### `docs/optimization_02_array_ladder.md`

Records the largest reported improvement after replacing `std::map` price levels with direct arrays/bitmaps. The mechanism is plausible: arithmetic indexing, contiguous levels, and fewer tree nodes. The file does not establish controlled source/build/hardware provenance, and its “O(1)” descriptions must include bitmap-scan and range assumptions. Study with HEL-027/036/043.

#### `docs/optimization_03_hashmap_attempt.md`

Documents a reverted custom/open-addressing order-ID map that performed worse on the recorded workload. This is valuable engineering behavior—reversing an optimization—but the implementation and exact experiment are absent, so causes such as probing, load factor, hash, deletion, layout, or integration cannot be independently tested. Treat the causal explanation as inference and the numbers as unverified historical claims (HEL-045).

### Test-output artifacts

#### `docs/aligned_results.txt`

Captured output from 13 GTests after an alignment-era variant. It reports basic test passes and in-test performance such as 317.4 ns/op for 100k Adds and a printed maximum. It proves only that those named assertions passed in an unidentified historical build. Because current tests contain a tautological invariant and weak FIFO case, “13 passed” is not full structural proof. Timing includes the contamination described in the per-file test guide.

#### `docs/pooled_results.txt`

Another 13-test capture associated by filename with pool work. Some printed metrics are slower than `aligned_results.txt`, illustrating why individual mixed test timings are noisy and non-causal. No manifest identifies compiler/source/hardware. Both files connect to HEL-009/011/033/043/048.

### Manual-workload artifacts

#### `docs/basline_million_ops.txt`

Captured Demo 3 output for one million generated operations, including progress printing, 70/25/5 advertised action mix, and final state. The filename misspells “baseline.” The driver timing includes RNG, ID-vector bookkeeping, progress I/O, and stale-ID cancel attempts after sweeps. Its 414 ns/op is a whole-driver historical average, not book-operation latency.

#### `docs/pooled_million_ops.txt`

Same output schema and deterministic operation counts with a reported 285 ns/op, suggesting an associated pool-era change. Equal seeds/counts aid comparison, but source revision, exact flags, machine state, and raw timing are absent. The delta is an observation, not proof that pooling alone caused it. Study with HEL-034/043/048.

### RDTSC benchmark artifacts

#### `docs/bench_unaligned.txt`

Historical calibrated-TSC output labeled unaligned: batch Add mean 102.7 ns and aggregated percentiles. It reports “pinned” without evidence that affinity succeeded and labels median-of-seven maxima as “worst single sample.”

#### `docs/bench_aligned.txt`

Historical aligned variant: batch 104.8 ns, close/slightly slower than unaligned in that run, which does not establish benefit from 64-byte alignment. This is the most direct repository evidence that the alignment hypothesis was not isolated convincingly (HEL-015).

#### `docs/bench_array.txt`

Historical direct-array variant: batch 26.6 ns and much lower central quantiles. It motivates the ladder design, but cannot be reproduced solely from tracked manifest data. It remains a result from a stated harness, not a universal order-book latency or causal proof. All three share HEL-005/029–031/033/043/048 caveats.

### Flamegraph placeholders

All seven tracked `.svg` files contain one plain-text error line rather than SVG markup or profiling stacks:

- `docs/flamegraph.svg` — “Unable to render block”
- `docs/flamegraph_itch.svg`
- `docs/flamegraph_itch_FULL.svg`
- `docs/flamegraph_itch_prefaulted.svg`
- `docs/flamegraph_itch_prefaulted_FULL.svg`
- `docs/flamegraph_itch_uint64.svg`
- `docs/flamegraph_itch_uint64_FULL.svg` — each says “Unable to render code block”

They provide no visual or quantitative evidence. Their names reveal intended experiments—synthetic/general, ITCH, full, prefaulted, and packed-`uint64` symbol comparison—but the actual profiles are missing. This is repository evidence for HEL-014, not a reason to infer what the flames showed.

### Replay-report artifacts

#### `docs/itch_replay_2026-06-09_003448.md`

Script-style report for `12302019.NASDAQ_ITCH50` and AAPL, recording binary path, one file/symbol, console output, exit code, and elapsed seconds. It lacks input/binary hashes, source revision, dirty status, compiler flags, machine/OS, page-cache state, and parser outcome taxonomy. Its output schema does not align fully with current source, supporting HEL-007.

#### `docs/itch_replay_2026-06-09_003836.md`

Analogous report for `sample_500mb.ITCH50`. It is useful as a historical workload witness, not current correctness evidence. Both reports inherit the cents-collapse defect if produced by equivalent adapter code and may count “modeled” callbacks rather than all frames or accepted mutations. Study with HEL-001/007/023/025/048.

### Profiling notes

#### `docs/profiling_notes.md`

Describes a `perf` workflow over 200 million synthetic mixed operations, records 9.5 seconds, and attributes shares to RNG, book mutation, hash/map, and other work. The important insight is that workload generation occupies the same profile; conclusions about book-only hot functions must subtract or separately measure the driver. The build/run command is historical and not a full manifest (HEL-032/043).

#### `docs/profiling_NASDAQ_sample500MB.md`

Records method and output for a 500 MB real-feed slice, including mmap/prefault intent and final AAPL state. It discusses why parser, symbol filtering, map, and book work appear. Counts/results cannot be reconciled completely with current output/source, and Price(4) precision/range defects affect semantic validity. The prefault interpretation must be checked against actual OS calls/page residency (HEL-001/007/026/044).

#### `docs/profiling_real_NASDAQ.md`

Analogous notes for a 7.7 GB full capture. It is evidence that the project was exercised on a large historical input and that whole-file ingestion matters; it is not proof of complete session correctness, since transport sequence/gap/recovery and full protocol message coverage are absent. Large-file cache/residency state materially affects timings. Study with HEL-023/026/033/037/043/048.

## Intentionally excluded material

No Git-tracked path is excluded. Generated files outside `git ls-files` are not line-reviewed unless surfaced by the tracked evidence narrative; opaque binaries, build caches, dependency indexes, Git internals, and raw market-data captures are intentionally outside this textbook. `ENGINEERING_BACKLOG.md` is untracked but was read first as required and used to keep HEL identifiers/priorities consistent; it is planning input, not part of the 48-file tracked inventory.

## Coverage invariants

1. Every row in `git ls-files` appears exactly once in the inventory table.
2. Every tracked code/header/test/benchmark/build/script path links to a dedicated guide with the required 15 sections.
3. Every tracked documentation/artifact path has a purpose, evidence classification, limitations, and shared-chapter connection here.
4. Every HEL-001–HEL-048 identifier appears in [12](12-technical-debt-and-limitations.md).
