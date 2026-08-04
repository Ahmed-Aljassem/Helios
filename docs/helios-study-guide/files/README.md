# Per-file Walkthrough Index

These guides follow the tracked source in line order and link shared concepts back to the textbook chapters. They do not reproduce large code blocks. Line ranges refer to the audited revision and may drift after future edits.

## Build and configuration

- [Root `.gitignore`](config-gitignore.md)
- [`CMakeLists.txt`](CMakeLists.md)
- [`run_itch_replays.sh`](run-itch-replays-sh.md)

## Headers

- [`include/types.hpp`](include-types-hpp.md)
- [`include/order.hpp`](include-order-hpp.md)
- [`include/price_level.hpp`](include-price-level-hpp.md)
- [`include/object_pool.hpp`](include-object-pool-hpp.md)
- [`include/orderbook.hpp`](include-orderbook-hpp.md)
- [`include/itch_parser.hpp`](include-itch-parser-hpp.md)
- [`include/book_replay.hpp`](include-book-replay-hpp.md)
- [`include/rdtsc_timer.hpp`](include-rdtsc-timer-hpp.md)

## Implementation files

- [`src/order.cpp`](src-order-cpp.md)
- [`src/price_level.cpp`](src-price-level-cpp.md)
- [`src/orderbook.cpp`](src-orderbook-cpp.md)

## Benchmarks and profiling drivers

- [`benchmarks/benchmark_orderbook.cpp`](benchmarks-benchmark-orderbook-cpp.md)
- [`benchmarks/profile_driver.cpp`](benchmarks-profile-driver-cpp.md)
- [`benchmarks/itch_replay.cpp`](benchmarks-itch-replay-cpp.md)
- [`benchmarks/itch_book_replay.cpp`](benchmarks-itch-book-replay-cpp.md)

## Tests and diagnostic programs

- [`tests/test_orderbook.cpp`](tests-test-orderbook-cpp.md)
- [`tests/manual_test.cpp`](tests-manual-test-cpp.md)
- [`tests/alloc_check.cpp`](tests-alloc-check-cpp.md)
- [`tests/test_order.cpp`](tests-test-order-cpp.md)
- [`tests/test_price_level.cpp`](tests-test-price-level-cpp.md)

The tracked documentation and evidence artifacts are mapped in [Tracked File Coverage](../TRACKED_FILE_COVERAGE.md). They are interpreted in shared chapters rather than receiving redundant walkthrough files.
