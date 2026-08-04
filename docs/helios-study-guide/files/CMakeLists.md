# File Guide: `CMakeLists.txt`

## 1. File purpose

Defines the C++ build graph for the order-book library, tests, manual driver, microbenchmark, parser replay, and book replay.

## 2. Architecture position

It turns source modules into linkable targets. The full graph is in [10](../10-build-system-and-tooling.md).

## 3. Dependencies

Requires CMake 3.14+, a C++ compiler, threads, and GoogleTest at configure time.

## 4. Symbols defined

CMake project `UltraLowLatencyOrderBook`; variable `SOURCES`; targets `orderbook_lib`, `test_orderbook`, `manual_test`, `benchmark_orderbook`, `itch_replay`, `itch_book_replay`; CTest `OrderBookTests`.

## 5. Symbols consumed

Tracked `.cpp` files, `include/`, GTest variables/libraries, `pthread`, compiler flags, and CTest.

## 6. Source-order walkthrough

- **Lines 1–2:** establish minimum CMake version and project identity; no language is explicitly constrained in `project`.
- **Lines 4–6:** require C++17, contradicting README's C++20 statement (HEL-010).
- **Lines 8–11:** append global warnings/O2, then define configuration strings. Flags apply per compilation action; a benchmark's private O3 option does not retroactively rebuild `orderbook_lib`.
- **Lines 13–14:** global include path leaks directory policy to every target.
- **Lines 16–23:** only `orderbook.cpp` and `price_level.cpp` form the library; header-only parser, pool, replay, types, timer, and `Order` definitions compile in consumers. `src/order.cpp` is excluded.
- **Lines 25–33:** testing is always enabled, GTest is mandatory, and only `tests/test_orderbook.cpp` is registered.
- **Lines 35–42:** manual driver and primary benchmark link the library; private flags optimize only the benchmark translation unit.
- **Lines 45–51:** parser-only replay is header-only and links no library; book replay links the library. Profiling and allocation-check sources are excluded.

## 7. Inputs and outputs

Inputs are source/toolchain/configuration. Outputs are build-system files, object files, one static/shared-by-default-context library, executables, and CTest metadata.

## 8. Ownership and lifetime

CMake generates build files; the selected generator owns incremental dependency scheduling. Targets own their compilation properties.

## 9. Invariants

Every linked symbol needs one definition; headers must be reachable; all translation units in a claimed experiment need recorded flags; registered tests must be discoverable.

## 10. Errors and edge cases

Missing GTest blocks all configuration. Multi-config generators treat flags differently from single-config builds. `-march=native` reduces portability. Global strings can override/duplicate toolchain policy.

## 11. Performance implications

Optimization, architecture selection, `NDEBUG`, and translation-unit boundaries materially change generated code and benchmark interpretation.

## 12. Technical debt

Global flags, mandatory tests, absent options/presets/export, excluded diagnostic targets, no CI matrix, and provenance ambiguity.

## 13. Related findings

HEL-005, HEL-010, HEL-011, HEL-040, HEL-046, HEL-047.

## 14. Interview questions

- Why does O3 on `benchmark_orderbook` not imply O3 inside `orderbook_lib`?
- How would target usage requirements differ from global include/flag state?
- Which tracked source files are not in any target?

## 15. Exercises

Draw every translation unit and the exact flags it receives for no build type, Debug, and Release. Predict which inline header functions inherit consumer flags.
