# 02 — Complete Architecture

## System view

```mermaid
flowchart LR
    BF[Historical BinaryFILE] --> MM[mmap region]
    MM --> Frame[parseBuffer framing]
    Frame --> Decode[ITCH decode]
    Decode --> Msg[stack Message]
    Msg --> Replay[BookReplay]
    Replay --> Book[OrderBook]
    Book --> IDs[unordered_map ID index]
    Book --> Ladders[bid/ask ladders]
    Ladders --> Levels[PriceLevel queues]
    Book --> Bits[occupancy bitmaps]
    Book --> Pool[ObjectPool]
    Pool --> Orders[Order nodes]
    Orders --> IDs
    Orders --> Levels
```

The architecture deliberately separates byte interpretation, feed-event semantics, and generic book mechanics. That separation is good. The price scale crossing from replay into the book is currently a leaky boundary.

## Directory structure and ownership of concerns

```text
Helios/
├── include/       public/domain headers; parser, replay, pool, timer are header-only
├── src/           out-of-line PriceLevel and OrderBook mutations; Order stub
├── benchmarks/    microbenchmark, profiler driver, parser replay, book replay
├── tests/         registered GTest, manual demos, allocation experiment, stubs
├── docs/          historical architecture, optimization, profile, and result evidence
├── CMakeLists.txt build graph and compiler policy
├── run_itch_replays.sh experiment orchestration
└── README.md      public narrative and historical headline claims
```

The directory names do not perfectly equal build boundaries. `include/itch_parser.hpp`, `include/book_replay.hpp`, `include/object_pool.hpp`, and `include/rdtsc_timer.hpp` contain implementations compiled into consumers. `benchmarks/profile_driver.cpp`, `tests/alloc_check.cpp`, `src/order.cpp`, and two stub test sources are tracked but absent from the CMake graph. The `docs/` directory mixes explanation with historical generated evidence; [the coverage catalog](TRACKED_FILE_COVERAGE.md) classifies every artifact.

## Module responsibilities

| Module | Responsibility | Does not own |
|---|---|---|
| `types.hpp` | Scalar domains and formatting/conversion helpers | State |
| `Order` | One live order plus queue links | Its neighbors |
| `PriceLevel` | FIFO topology and level aggregates | Order storage |
| `ObjectPool` | Raw chunks, construction slots, free list | Hash/list indexes |
| `OrderBook` | Coordinates all indexes and derived state | External callers/replay |
| `itch_parser.hpp` | Frame and decode bytes | Mapped bytes after callback |
| `BookReplay` | Map modeled ITCH events into one book | Book or input buffer |
| replay executable | File descriptor and mapping lifecycle | Order internals |

## Dependency graph

```mermaid
flowchart TD
    Types[types.hpp] --> Order[order.hpp]
    Order --> PLH[price_level.hpp]
    PLH --> PLC[src/price_level.cpp]
    Pool[object_pool.hpp] --> OBH[orderbook.hpp]
    PLH --> OBH
    OBH --> OBC[src/orderbook.cpp]
    Parser[itch_parser.hpp] --> BR[book_replay.hpp]
    OBH --> BR
    Timer[rdtsc_timer.hpp] --> Bench[benchmark_orderbook.cpp]
    OBC --> Lib[orderbook_lib]
    PLC --> Lib
    Lib --> Bench
    Lib --> Tests[test_orderbook]
    Lib --> Manual[manual_test]
    Lib --> ReplayExe[itch_book_replay]
    BR --> ReplayExe
    Parser --> CountExe[itch_replay]
```

Header-only modules move implementation into every consuming translation unit. `OrderBook` and `PriceLevel` non-inline methods are compiled into `orderbook_lib`.

## Source-file relationship graph

```mermaid
flowchart LR
    T[include/types.hpp] --> O[include/order.hpp]
    O --> PLH[include/price_level.hpp]
    PLH --> PLC[src/price_level.cpp]
    PLH --> OBH[include/orderbook.hpp]
    OP[include/object_pool.hpp] --> OBH
    OBH --> OBC[src/orderbook.cpp]
    IP[include/itch_parser.hpp] --> BR[include/book_replay.hpp]
    OBH --> BR
    RT[include/rdtsc_timer.hpp] --> BB[benchmarks/benchmark_orderbook.cpp]
    OBH --> BB
    IP --> IR[benchmarks/itch_replay.cpp]
    BR --> IBR[benchmarks/itch_book_replay.cpp]
    OBH --> PD[benchmarks/profile_driver.cpp]
    OBH --> TO[tests/test_orderbook.cpp]
    OBH --> MT[tests/manual_test.cpp]
    OBH --> AC[tests/alloc_check.cpp]
    Stub[src/order.cpp + two test stubs] -. no current dependency .-> OBH
```

The arrows mean source-level inclusion/consumption, not ownership. For example, `BookReplay` includes the parser and book types but owns neither decoded callback objects nor the referenced book. The build graph below is intentionally different: not every relationship becomes a separately compiled target edge.

## Build graph

```mermaid
flowchart TB
    CMake[CMakeLists.txt] --> Lib[orderbook_lib]
    Lib --> T1[test_orderbook]
    Lib --> T2[manual_test]
    Lib --> B1[benchmark_orderbook]
    Lib --> B2[itch_book_replay]
    CMake --> B3[itch_replay]
    Omitted[profile_driver / alloc_check / stubs] -. not targets .-> CMake
```

The current generated commands show `gnu++17`. In a no-build-type local configuration, `orderbook_lib` receives global `-O2`; the benchmark translation unit additionally receives `-O3 -march=native`. Executable flags do not retroactively recompile the library.

## Ownership graph

```mermaid
flowchart TD
    Main[replay main stack] -->|owns| FD[file descriptor]
    Main -->|owns lifetime| Map[mapped bytes]
    Main -->|owns| Book[OrderBook]
    Main -->|owns| RP[BookReplay]
    RP -->|borrows reference| Book
    Parser[parseBuffer stack] -->|borrows span| Map
    Parser -->|owns temporarily| Message[Message]
    Callback -->|borrows during call| Message
    Book -->|owns| Pool[ObjectPool chunks]
    Book -->|owns nodes/buckets| Hash[unordered_map]
    Book -->|owns| Arrays[ladders + bitmaps]
    Pool -->|owns storage| Order[Order]
    Hash -->|observes| Order
    Arrays -->|PriceLevel observes| Order
```

No smart pointer owns an individual live `Order`; the pool owns its storage collectively. Hash and level pointers are non-owning.

## Data-flow boundary table

| Boundary | Data crossing | Validity | Errors today | Cleanliness |
|---|---|---|---|---|
| File → mapping | OS virtual-memory region | Until `munmap` | Several calls unchecked | Leaky operational boundary |
| Mapping → framing | byte pointer + size | Parsing call | Truncation silently stops | Incomplete error contract |
| Framing → decode | message body + declared length | Decode call | `false` conflates cases | Leaky |
| Decode → callback | const `Message&` | Callback only | No typed error | Lifetime clean, error weak |
| Callback → replay | normalized fields | Callback only | Missing order silently ignored | Policy implicit |
| Replay → book | side, cents price, quantity, ID | Mutation | precision/range loss | Semantically leaky |
| Book → structures | indexes/pointers/counters | Book lifetime | no invariant checker | Strong coordination required |

## Control-flow graph: modeled replay

```mermaid
flowchart TD
    Start --> Len[Read 2-byte length]
    Len -->|zero| Stop[Stop at terminator]
    Len -->|tail too short| Trunc[Stop silently]
    Len --> Type[Decode type]
    Type -->|unmodeled| Advance[Advance frame]
    Type -->|modeled| Callback[BookReplay::onMessage]
    Callback --> Switch{A/F, D, X, E/C, U}
    Switch --> Add[filter symbol then add]
    Switch --> Delete[cancel if present]
    Switch --> Reduce[modify or cancel]
    Switch --> Replace[cancel old, add new]
    Add --> Advance
    Delete --> Advance
    Reduce --> Advance
    Replace --> Advance
    Advance --> Len
```

## Generated artifacts

The workspace contains CMake caches, dependency files, object files, executables, a static library, test logs, compile commands, and clangd indexes. They are not authored modules. Useful evidence includes:

- `compile_commands.json`: actual per-translation-unit flags;
- `flags.make` and `link.txt`: target build behavior;
- `LastTest.log`: what the registered test target ran;
- binary symbols: which compiled methods entered each executable.

Opaque indexes and binaries are not educational source substitutes.

## Boundary defense questions

1. Why does `BookReplay` depend on both parser and book types?
2. Would a typed variant of message structs make the parser/replay boundary cleaner?
3. Why is the `Message&` callback lifetime safe now, and how could storing it become unsafe?
4. Where should price-scale conversion live?
5. Which layer should detect update-before-add anomalies?
