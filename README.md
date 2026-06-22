# Helios

Measured low-latency C++20 limit order book and NASDAQ TotalView-ITCH 5.0 replay engine focused on deterministic replay, latency profiling, and market-data infrastructure.

## Highlights

- 37.57M ops/sec throughput

- 26.6 ns mean latency

- 28x reduction in worst-case latency

- 13.8M NASDAQ ITCH messages replayed

- Direct-mapped price ladder + occupancy bitmap

- Intrusive FIFO queues

- Custom object pool allocator

## Current Status

Helios currently implements the software order-book core and a real-market-data replay path:

- In-memory limit order book with add, cancel, modify, best bid/ask, spread, and market-order execution.
- Fixed-point integer prices and 64-bit order identifiers.
- Intrusive FIFO queues at each price level.
- Pooled `Order` allocation to avoid per-order `new`/`delete` on the hot path.
- Direct-mapped bid/ask price ladders with occupancy bitmaps.
- NASDAQ TotalView-ITCH 5.0 parser for the modeled order-event messages used by replay.
- Single-symbol ITCH replay that maps exchange order references into the book.
- RDTSC benchmark harness for x86/Linux latency measurement.
- GoogleTest suite covering core order-book behavior and stress scenarios.

Not implemented today: kernel-bypass networking, lock-free feed ingestion, dashboards, FPGA parsing, production multi-symbol sharding, or a full aggressive limit-order matching path. Those are roadmap items only.

## Performance Snapshot

Primary saved benchmark artifact: [`docs/bench_array.txt`](docs/bench_array.txt)

Environment recorded in the project docs:

- OS/kernel: Ubuntu Linux
- CPU: x86-64, 6 cores, 12 Threads
- Benchmark pinned to core 3
- TSC frequency in run: 3.1104 GHz
- Build mode: optimized, `-O3 -march=native`
- Measurement: RDTSC harness, timer overhead subtracted, median of 7 runs

Steady-state `addOrder` benchmark:

| Metric | Result |
|---|---:|
| Mean | 26.6 ns/op |
| Throughput | 37.57 M ops/s |
| p50 | 25.1 ns |
| p90 | 28.6 ns |
| p99 | 37.3 ns |
| p99.9 | 98.7 ns |
| Max sample | 23,964.5 ns |

The max sample is intentionally reported, not hidden. The profiling notes attribute the remaining tail to platform effects such as first-touch faults, interrupts, or OS/hardware jitter rather than a recurring order-book code path.

## Architecture Overview

The order book is built around a few narrow data-structure choices:

| Question | Structure | Why |
|---|---|---|
| Where is order `N`? | `std::unordered_map<OrderId, Order*>` | O(1) average lookup for cancel, modify, reduce, and replace. |
| What is at price `P`? | Direct price ladder: `price - min_price` | O(1) price-level access without tree walks. |
| What is the best bid/ask? | Occupancy bitmap plus bit scan | Fast best-price refresh when a level becomes empty. |
| Who has time priority at one price? | Intrusive doubly linked list | O(1) append and O(1) removal without extra list-node allocation. |
| Where do orders live? | `ObjectPool<Order>` | Reuses slab-allocated order storage and improves locality. |

High-level flow:

```text
Manual/API path:
  caller -> OrderBook -> PriceLevel -> pooled Order storage

ITCH replay path:
  ITCH file -> mmap -> itch::parseBuffer -> BookReplay -> OrderBook
```

Important files:

- [`include/orderbook.hpp`](include/orderbook.hpp) and [`src/orderbook.cpp`](src/orderbook.cpp): core book state and operations.
- [`include/price_level.hpp`](include/price_level.hpp) and [`src/price_level.cpp`](src/price_level.cpp): intrusive FIFO queue per price.
- [`include/object_pool.hpp`](include/object_pool.hpp): chunked object pool.
- [`include/itch_parser.hpp`](include/itch_parser.hpp): framed ITCH parser with explicit big-endian field reads.
- [`include/book_replay.hpp`](include/book_replay.hpp): one-symbol ITCH-to-book replay adapter.
- [`benchmarks/benchmark_orderbook.cpp`](benchmarks/benchmark_orderbook.cpp): calibrated RDTSC benchmark.
- [`benchmarks/itch_book_replay.cpp`](benchmarks/itch_book_replay.cpp): real ITCH replay executable.

## What Is Implemented

### Order Book Core

- `addOrder(side, price, quantity)`
- `addOrder(side, price, quantity, external_id)` for exchange-supplied order references
- `cancelOrder(id)`
- `modifyOrder(id, new_quantity)`
- `executeMarketOrder(side, quantity)`
- `getBestBid()`, `getBestAsk()`, `getSpread()`
- Active order and active price-level counters

The book stores fixed-point prices as integer ticks. In the default convention, `10050` represents `100.50`.

### ITCH Parser and Replay

The parser handles historical ITCH files where each message is framed by a 2-byte big-endian length. Modeled message types:

- `A`: Add Order
- `F`: Add Order with MPID
- `D`: Delete
- `X`: Cancel / reduce shares
- `E`: Executed
- `C`: Executed with price
- `U`: Replace

Other message types are skipped while preserving stream framing.

`BookReplay` filters one target symbol. Add messages carry the symbol; later delete/cancel/execute/replace messages refer to exchange order IDs, so non-target orders are naturally ignored because they were never inserted.

### Tests

The active CMake test target is `test_orderbook`, built from [`tests/test_orderbook.cpp`](tests/test_orderbook.cpp). It covers:

- Add bid/ask behavior
- Best bid/ask/spread
- Cancel
- Market-order full and partial fills
- FIFO behavior at one price
- 100k to 1M order stress scenarios
- Mixed add/cancel and market-order stress scenarios

## Benchmarking Methodology

The benchmark harness is designed to avoid common microbenchmark mistakes:

- Calibrates TSC frequency at runtime.
- Uses `RDTSC`/`RDTSCP` with fences on x86.
- Measures and subtracts timer overhead.
- Pre-generates inputs so random-number generation is not inside the timed region.
- Warms up the book, caches, branch predictor, and CPU frequency.
- Pins the benchmark thread to one core on Linux.
- Reports both batch mean and per-operation latency percentiles.
- Repeats the distribution run and reports median-of-7 results.

The RDTSC benchmark is meaningful on x86/Linux. On non-x86 systems, including this repository's current macOS/ARM64 development host, the benchmark falls back to a no-op clock and reports invalid `0.0 ns` results. Build and tests still run there; latency numbers should be collected on x86/Linux.

## Real NASDAQ ITCH Replay

Saved artifact: [`docs/itch_replay_2026-06-09_003836.md`](docs/itch_replay_2026-06-09_003836.md)

Input:

- File: `sample_500mb.ITCH50`
- Size in report: 477 MB
- Symbol: `AAPL`

Replay result:

```text
modeled messages parsed : 13,838,945
adds=50,364 deletes=26,200 reduces=3,660 replaces=4,948

final book state for AAPL:
  live orders   : 21,948
  bid levels    : 3,597
  ask levels    : 1,135
  best bid      : $287.53
  best ask      : $287.58
  spread        : $0.05
  crossed?      : no
```

The non-crossed final book is a useful integration check: the parser, symbol filter, exchange order-reference mapping, and book updates are coherent enough to reconstruct a plausible single-symbol book from real feed data.

A full-day replay artifact is also present in [`docs/itch_replay_2026-06-09_003448.md`](docs/itch_replay_2026-06-09_003448.md). It parses 263,241,937 modeled messages for AAPL and ends with zero live orders, which is consistent with an end-of-session drain. A timestamp cutoff is needed for cleaner intraday snapshots.

Market-data files are not committed to the repository.

## Key Optimization Story

The project keeps both successful and failed optimizations in the docs because the main goal is disciplined measurement, not benchmark theater.

### 1. Object Pool Allocation

[`docs/optimization_01_memory_pool.md`](docs/optimization_01_memory_pool.md)

Replacing per-order heap allocation with `ObjectPool<Order>` improved the early latency distribution:

| Metric | Baseline | Pooled |
|---|---:|---:|
| p50 | 187 ns | 81 ns |
| p99 | 423 ns | 124 ns |
| p99.9 | 2,657 ns | 438 ns |

### 2. `std::map` to Direct Price Ladder

[`docs/optimization_02_array_ladder.md`](docs/optimization_02_array_ladder.md)

Replacing ordered price-level maps with direct arrays and bitmaps was the largest measured win:

| Metric | `std::map` | Array + bitmap |
|---|---:|---:|
| Mean | 104.8 ns | 26.6 ns |
| Throughput | 9.55 M ops/s | 37.57 M ops/s |
| Max sample | 665,081 ns | 23,964.5 ns |

This removed tree walks, scattered tree nodes, and per-price-level allocation from the hot path.

### 3. Custom Hash Table Attempt, Reverted

[`docs/optimization_03_hashmap_attempt.md`](docs/optimization_03_hashmap_attempt.md)

A hand-rolled open-addressing map was tested as a replacement for `std::unordered_map`. It was worse:

| Metric | `std::unordered_map` | Open-addressing attempt |
|---|---:|---:|
| Mean | 26.6 ns | 32.5 ns |
| p50 | 25.1 ns | 97.7 ns |
| p99 | 37.3 ns | 265.2 ns |
| p99.9 | 98.7 ns | 561.3 ns |
| Max | 23,964 ns | 26,744 ns |

The result is useful: the obvious "HFT-style" data structure was not automatically better. The implementation was reverted.

### 4. Real-Feed Profiling

[`docs/profiling_notes.md`](docs/profiling_notes.md) and [`docs/profiling_NASDAQ_sample500MB.md`](docs/profiling_NASDAQ_sample500MB.md)

Synthetic steady-state profiling over 200M operations found:

- IPC around 1.78.
- Cache misses around 0.09 per operation.
- 67 context switches and 0 CPU migrations.
- 37,353 page faults, all minor.

The project notes conclude that the remaining approximately 24 us outlier is not a recurring algorithmic bottleneck.

Real-feed profiling showed that parsing and file I/O dominate much more than book updates. A symbol-filter optimization changed per-add `strcmp` into an 8-byte `uint64_t` comparison after profiling showed `strcmp` was a significant cost.

## Build, Test, Run

Requirements:

- CMake 3.14+
- A modern C++ compiler
- GoogleTest discoverable by CMake
- Linux/x86 for meaningful RDTSC benchmark results and `perf` profiling

Build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Run tests:

```bash
ctest --test-dir build --output-on-failure
```

Run the benchmark on Linux/x86:

```bash
./build/benchmark_orderbook 3
```

The optional argument is the core number to pin to. On non-Linux platforms, affinity pinning is skipped. On non-x86 platforms, RDTSC timing is not meaningful.

Parse an ITCH file and print modeled message counts:

```bash
./build/itch_replay /path/to/file.ITCH50
```

Replay one symbol into the book:

```bash
./build/itch_book_replay /path/to/file.ITCH50 AAPL
```

Batch replay reports:

```bash
./run_itch_replays.sh -s AAPL,MSFT,SPY -b ./build/itch_book_replay /path/to/*.NASDAQ_ITCH50
```

Generated reports are written under `docs/`.

## Roadmap

Near-term:

- Update the CMake baseline if the project should publicly target C++20.
- Add focused tests for `PriceLevel`, `Order`, `ObjectPool`, ITCH parsing, and replay edge cases.
- Strengthen invariant checks under randomized operation sequences.
- Add duplicate external-order-ID handling.
- Implement `printBook` or remove the presentation API.
- Add intraday timestamp cutoff support for ITCH replay snapshots.
- Wire `profile_driver.cpp` and allocation checks into CMake.
- Regenerate real flamegraphs or remove placeholder SVG files.

Later:

- Add aggressive limit-order matching semantics.
- Support multi-symbol replay with explicit book ownership/sharding.
- Add low-overhead metrics export.
- Build a real-time dashboard for latency and book-state visualization.
- Explore FPGA/RTL packet parsing and compare it against the software parser.
- Explore lock-free or kernel-bypass feed ingestion only after the single-threaded replay path is fully measured.

## What I Learned

The main engineering lesson from Helios is that low-latency work has to be measurement-driven.

Several changes matched intuition: pooling orders helped, and replacing `std::map` with a direct ladder was a large win. Other ideas did not: the custom open-addressing hash map was slower than `std::unordered_map`, and profiling showed that the remaining tail was more likely platform noise than a data-structure problem.

That is the point of the project: use low-level systems techniques, but keep the benchmark harness honest enough to reject appealing ideas when the measurements do not support them.

## Deep Dive / Design Notes

- [`docs/optimization_01_memory_pool.md`](docs/optimization_01_memory_pool.md): object pool results.
- [`docs/optimization_02_array_ladder.md`](docs/optimization_02_array_ladder.md): direct price ladder results.
- [`docs/optimization_03_hashmap_attempt.md`](docs/optimization_03_hashmap_attempt.md): reverted hash-map experiment.
- [`docs/profiling_notes.md`](docs/profiling_notes.md): synthetic `perf` profiling summary.
- [`docs/profiling_NASDAQ_sample500MB.md`](docs/profiling_NASDAQ_sample500MB.md): 500 MB NASDAQ replay profiling.
- [`docs/profiling_real_NASDAQ.md`](docs/profiling_real_NASDAQ.md): full-day NASDAQ replay profiling.
- [`PROJECT_OVERVIEW.md`](PROJECT_OVERVIEW.md): repo audit notes, TODOs, source tree, and rough project summary.

[`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md) is historical and currently stale; it still describes an older `std::map` design rather than the current direct-ladder implementation.
