# 10 — Build System and Tooling

## CMake walkthrough

### Project and language

`cmake_minimum_required(3.14)` sets the oldest declared CMake. `project(UltraLowLatencyOrderBook VERSION 1.0)` names the build project. `CMAKE_CXX_STANDARD 17` and `REQUIRED ON` request C++17; this conflicts with the README’s C++20 label.

### Global flags

The file appends `-Wall -Wextra -O2` to global C++ flags, then assigns Debug and Release flag variables. This is an old-style global approach:

- it affects unrelated targets;
- it can conflict with toolchain/user flags;
- multiple `-O` flags can appear;
- architecture-specific `-march=native` reduces binary portability.

Target-scoped `target_compile_options` and properties make ownership clearer.

### Include path

`include_directories(include)` globally exposes project headers. A professional library would normally attach include paths to `orderbook_lib` with public/private scope.

### Library

`SOURCES` contains only `src/orderbook.cpp` and `src/price_level.cpp`. `add_library(orderbook_lib ...)` creates the core library. `src/order.cpp` is not compiled.

### Tests

`enable_testing`, required GTest discovery, and `add_test` register one `test_orderbook` executable. GTest is mandatory even for users who only want library or replay tools. Separate unfinished tests and allocation check are absent.

### Other executables

- `manual_test` links the library.
- `benchmark_orderbook` links the library; its own translation unit gets O3/native.
- `itch_replay` is header-only parser code and does not link the library.
- `itch_book_replay` links the library and includes header-only parser/replay code.

`profile_driver.cpp` is absent from CMake despite documentation showing manual compilation.

## Translation units

A translation unit is a source file after preprocessing its included headers. `src/orderbook.cpp` is compiled separately from `benchmark_orderbook.cpp`.

Therefore:

```text
benchmark_orderbook.cpp compiled -O3
orderbook.cpp compiled -O2
link both
```

does not mean `OrderBook::addOrder` was O3-compiled. Whole-program/LTO could change optimization boundaries, but no LTO is configured.

## Debug versus Release

The audited local CMake cache had no build type, so global O2 applied to the library. Debug adds `-g -O0` after global O2; the later optimization option generally controls effective level. Release adds O3/native after O2.

The canonical invocation should name build type and preserve compile commands for provenance.

## Sanitizers

A temporary audit build demonstrated current tests compile and pass with ASan/UBSan on the macOS/ARM64 host. That is diagnostic evidence, not proof of all paths. Sanitizer flags must be applied consistently to compiled objects and link steps.

## Architecture flags

`-march=native` allows the compiler to use build-host instruction features and can make binaries nonportable. It is appropriate for controlled native benchmarks when recorded, not generic distributable artifacts.

## Link graph

Generated `link.txt` shows executables link the static `liborderbook_lib.a`; the GoogleTest executable also links GTest and pthread. Binary symbols confirm `OrderBook`/`PriceLevel` methods come from the library.

## Replay script

`run_itch_replays.sh`:

1. defines defaults;
2. parses options;
3. finds a replay binary;
4. gathers explicit files or `$HOME/market-data/*.NASDAQ_ITCH50`;
5. splits comma-separated symbols;
6. creates a timestamped report;
7. runs every file×symbol pair through `tee`;
8. records binary exit status and elapsed shell seconds.

It uses Bash arrays, `getopts`, `PIPESTATUS`, process substitution-like shell features, and writes generated reports into `docs` by default.

## Generated metadata worth reading

| Artifact | What it teaches |
|---|---|
| `CMakeCache.txt` | selected compiler, build type, discovered packages |
| `compile_commands.json` | exact compile command per source |
| `flags.make` | target flags under Makefiles generator |
| `link.txt` | objects/libraries used at link |
| `CTestTestfile.cmake` | registered test commands |
| `LastTest.log` | last observed test output |
| `.o.d` dependencies | actual header dependency closure |

## Files excluded from build

`src/order.cpp`, `tests/alloc_check.cpp`, `tests/test_order.cpp`, `tests/test_price_level.cpp`, and `benchmarks/profile_driver.cpp` are tracked but not ordinary CMake targets.

## Interview questions

1. Why do target flags not optimize a precompiled library?
2. What does `PUBLIC` versus `PRIVATE` include scope mean?
3. Why is mandatory GTest discovery inconvenient for library consumers?
4. When is `-march=native` appropriate?
5. Why should benchmark manifests include full compile commands rather than “built Release”?

