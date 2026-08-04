# 04 — Source and Build Architecture

The six views distinguish inclusion, compilation, linkage, test registration, and tracked-but-disconnected files.
<a id="a04-01"></a>
### A04-01 — Header dependencies

| Diagram card field | Value |
|---|---|
| Purpose and scope | Show header dependencies independently so one relationship is not mistaken for another. |
| Evidence/status | Direct CMake/include/source evidence. |
| Source evidence | [CMakeLists.txt](../../CMakeLists.txt), [orderbook.hpp](../../include/orderbook.hpp), [book_replay.hpp](../../include/book_replay.hpp), [order.cpp](../../src/order.cpp), [profile_driver.cpp](../../benchmarks/profile_driver.cpp), [alloc_check.cpp](../../tests/alloc_check.cpp) |
| Backlog | [INF-01](../../ENGINEERING_BACKLOG.md#inf-01--establish-a-compiler-and-verification-ci-matrix), [INF-02](../../ENGINEERING_BACKLOG.md#inf-02--modernize-target-scoped-cmake-behavior) |
| Findings | [HEL-010](11-current-technical-debt-overlay.md#hel-010), [HEL-011](11-current-technical-debt-overlay.md#hel-011), [HEL-046](11-current-technical-debt-overlay.md#hel-046), [HEL-047](11-current-technical-debt-overlay.md#hel-047) |
| Related atlas material | — |

```mermaid
flowchart LR
    T["[CURRENT] types.hpp"]:::current
    O["[CURRENT] order.hpp"]:::current
    P["[CURRENT] price_level.hpp"]:::current
    POOL["[CURRENT] object_pool.hpp"]:::current
    B["[CURRENT] orderbook.hpp"]:::current
    I["[CURRENT] itch_parser.hpp"]:::current
    R["[CURRENT] book_replay.hpp"]:::current
    T --> O
    O --> P
    T --> P
    P --> B
    POOL --> B
    B --> R
    I --> R
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - T --> O
  - O --> P
  - T --> P
  - P --> B
  - POOL --> B
  - B --> R
  - I --> R
- **What exists:** The solid nodes and edges are in tracked headers, translation units, or CMake targets.
- **What does not exist:** Disconnected dotted files do not participate in the configured build/test graph.

<a id="a04-02"></a>
### A04-02 — Translation units

| Diagram card field | Value |
|---|---|
| Purpose and scope | Show translation units independently so one relationship is not mistaken for another. |
| Evidence/status | Direct CMake/include/source evidence. |
| Source evidence | [CMakeLists.txt](../../CMakeLists.txt), [orderbook.hpp](../../include/orderbook.hpp), [book_replay.hpp](../../include/book_replay.hpp), [order.cpp](../../src/order.cpp), [profile_driver.cpp](../../benchmarks/profile_driver.cpp), [alloc_check.cpp](../../tests/alloc_check.cpp) |
| Backlog | [INF-01](../../ENGINEERING_BACKLOG.md#inf-01--establish-a-compiler-and-verification-ci-matrix), [INF-02](../../ENGINEERING_BACKLOG.md#inf-02--modernize-target-scoped-cmake-behavior) |
| Findings | [HEL-010](11-current-technical-debt-overlay.md#hel-010), [HEL-011](11-current-technical-debt-overlay.md#hel-011), [HEL-046](11-current-technical-debt-overlay.md#hel-046), [HEL-047](11-current-technical-debt-overlay.md#hel-047) |
| Related atlas material | — |

```mermaid
flowchart LR
    PL["[CURRENT] src/price_level.cpp"]:::current
    OB["[CURRENT] src/orderbook.cpp"]:::current
    OD["[DEBT] src/order.cpp NOT DONE/disconnected"]:::debt
    EX["[CURRENT] executable/test .cpp files"]:::current
    PL -->|compiled into library| L["[CURRENT] orderbook_lib"]:::current
    OB -->|compiled into library| L
    EX -->|one TU per target| X["[CURRENT] executables"]:::current
    OD -.->|not in SOURCES| X
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - PL -->|compiled into library| L["[CURRENT] orderbook_lib"]:::current
  - OB -->|compiled into library| L
  - EX -->|one TU per target| X["[CURRENT] executables"]:::current
  - OD -.->|not in SOURCES| X
- **What exists:** The solid nodes and edges are in tracked headers, translation units, or CMake targets.
- **What does not exist:** Disconnected dotted files do not participate in the configured build/test graph.

<a id="a04-03"></a>
### A04-03 — CMake targets

| Diagram card field | Value |
|---|---|
| Purpose and scope | Show cmake targets independently so one relationship is not mistaken for another. |
| Evidence/status | Direct CMake/include/source evidence. |
| Source evidence | [CMakeLists.txt](../../CMakeLists.txt), [orderbook.hpp](../../include/orderbook.hpp), [book_replay.hpp](../../include/book_replay.hpp), [order.cpp](../../src/order.cpp), [profile_driver.cpp](../../benchmarks/profile_driver.cpp), [alloc_check.cpp](../../tests/alloc_check.cpp) |
| Backlog | [INF-01](../../ENGINEERING_BACKLOG.md#inf-01--establish-a-compiler-and-verification-ci-matrix), [INF-02](../../ENGINEERING_BACKLOG.md#inf-02--modernize-target-scoped-cmake-behavior) |
| Findings | [HEL-010](11-current-technical-debt-overlay.md#hel-010), [HEL-011](11-current-technical-debt-overlay.md#hel-011), [HEL-046](11-current-technical-debt-overlay.md#hel-046), [HEL-047](11-current-technical-debt-overlay.md#hel-047) |
| Related atlas material | — |

```mermaid
flowchart LR
    L["[CURRENT] orderbook_lib"]:::current
    T["[CURRENT] test_orderbook"]:::current
    M["[CURRENT] manual_test"]:::current
    B["[CURRENT] benchmark_orderbook"]:::current
    I["[CURRENT] itch_replay"]:::current
    R["[CURRENT] itch_book_replay"]:::current
    L --> T
    L --> M
    L --> B
    L --> R
    I -.->|header-only parser; no library link| L
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - L --> T
  - L --> M
  - L --> B
  - L --> R
  - I -.->|header-only parser; no library link| L
- **What exists:** The solid nodes and edges are in tracked headers, translation units, or CMake targets.
- **What does not exist:** Disconnected dotted files do not participate in the configured build/test graph.

<a id="a04-04"></a>
### A04-04 — Linkage

| Diagram card field | Value |
|---|---|
| Purpose and scope | Show linkage independently so one relationship is not mistaken for another. |
| Evidence/status | Direct CMake/include/source evidence. |
| Source evidence | [CMakeLists.txt](../../CMakeLists.txt), [orderbook.hpp](../../include/orderbook.hpp), [book_replay.hpp](../../include/book_replay.hpp), [order.cpp](../../src/order.cpp), [profile_driver.cpp](../../benchmarks/profile_driver.cpp), [alloc_check.cpp](../../tests/alloc_check.cpp) |
| Backlog | [INF-01](../../ENGINEERING_BACKLOG.md#inf-01--establish-a-compiler-and-verification-ci-matrix), [INF-02](../../ENGINEERING_BACKLOG.md#inf-02--modernize-target-scoped-cmake-behavior) |
| Findings | [HEL-010](11-current-technical-debt-overlay.md#hel-010), [HEL-011](11-current-technical-debt-overlay.md#hel-011), [HEL-046](11-current-technical-debt-overlay.md#hel-046), [HEL-047](11-current-technical-debt-overlay.md#hel-047) |
| Related atlas material | — |

```mermaid
flowchart LR
    G["[CURRENT] GTest + pthread"]:::current
    L["[CURRENT] orderbook_lib"]:::current
    T["[CURRENT] test_orderbook"]:::current
    B["[CURRENT] benchmark/manual/replay-book"]:::current
    P["[CURRENT] itch_replay standalone"]:::current
    G -->|links| T
    L -->|links| T
    L -->|links| B
    P -.->|does not link library| L
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - G -->|links| T
  - L -->|links| T
  - L -->|links| B
  - P -.->|does not link library| L
- **What exists:** The solid nodes and edges are in tracked headers, translation units, or CMake targets.
- **What does not exist:** Disconnected dotted files do not participate in the configured build/test graph.

<a id="a04-05"></a>
### A04-05 — CTest registration

| Diagram card field | Value |
|---|---|
| Purpose and scope | Show ctest registration independently so one relationship is not mistaken for another. |
| Evidence/status | Direct CMake/include/source evidence. |
| Source evidence | [CMakeLists.txt](../../CMakeLists.txt), [orderbook.hpp](../../include/orderbook.hpp), [book_replay.hpp](../../include/book_replay.hpp), [order.cpp](../../src/order.cpp), [profile_driver.cpp](../../benchmarks/profile_driver.cpp), [alloc_check.cpp](../../tests/alloc_check.cpp) |
| Backlog | [INF-01](../../ENGINEERING_BACKLOG.md#inf-01--establish-a-compiler-and-verification-ci-matrix), [INF-02](../../ENGINEERING_BACKLOG.md#inf-02--modernize-target-scoped-cmake-behavior) |
| Findings | [HEL-010](11-current-technical-debt-overlay.md#hel-010), [HEL-011](11-current-technical-debt-overlay.md#hel-011), [HEL-046](11-current-technical-debt-overlay.md#hel-046), [HEL-047](11-current-technical-debt-overlay.md#hel-047) |
| Related atlas material | — |

```mermaid
flowchart LR
    C["[CURRENT] enable_testing"]:::current
    T["[CURRENT] test_orderbook executable"]:::current
    R["[CURRENT] OrderBookTests CTest entry"]:::current
    U["[DEBT] other test source files unregistered"]:::debt
    C --> R
    T -->|COMMAND| R
    U -.->|not add_executable/add_test| R
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - C --> R
  - T -->|COMMAND| R
  - U -.->|not add_executable/add_test| R
- **What exists:** The solid nodes and edges are in tracked headers, translation units, or CMake targets.
- **What does not exist:** Disconnected dotted files do not participate in the configured build/test graph.

<a id="a04-06"></a>
### A04-06 — Disconnected tracked files

| Diagram card field | Value |
|---|---|
| Purpose and scope | Show disconnected tracked files independently so one relationship is not mistaken for another. |
| Evidence/status | Direct CMake/include/source evidence. |
| Source evidence | [CMakeLists.txt](../../CMakeLists.txt), [orderbook.hpp](../../include/orderbook.hpp), [book_replay.hpp](../../include/book_replay.hpp), [order.cpp](../../src/order.cpp), [profile_driver.cpp](../../benchmarks/profile_driver.cpp), [alloc_check.cpp](../../tests/alloc_check.cpp) |
| Backlog | [INF-01](../../ENGINEERING_BACKLOG.md#inf-01--establish-a-compiler-and-verification-ci-matrix), [INF-02](../../ENGINEERING_BACKLOG.md#inf-02--modernize-target-scoped-cmake-behavior) |
| Findings | [HEL-010](11-current-technical-debt-overlay.md#hel-010), [HEL-011](11-current-technical-debt-overlay.md#hel-011), [HEL-046](11-current-technical-debt-overlay.md#hel-046), [HEL-047](11-current-technical-debt-overlay.md#hel-047) |
| Related atlas material | — |

```mermaid
flowchart LR
    O["[DEBT] src/order.cpp"]:::debt
    P["[DEBT] benchmarks/profile_driver.cpp"]:::debt
    A["[DEBT] tests/alloc_check.cpp"]:::debt
    TO["[DEBT] tests/test_order.cpp + test_price_level.cpp"]:::debt
    C["[CURRENT] CMake graph"]:::current
    O -.->|absent| C
    P -.->|absent| C
    A -.->|absent| C
    TO -.->|absent| C
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - O -.->|absent| C
  - P -.->|absent| C
  - A -.->|absent| C
  - TO -.->|absent| C
- **What exists:** The solid nodes and edges are in tracked headers, translation units, or CMake targets.
- **What does not exist:** Disconnected dotted files do not participate in the configured build/test graph.

<a id="a04-07"></a>
### A04-07 — Compile-flag pipeline

| Diagram card field | Value |
|---|---|
| Purpose and scope | Prove why executable-local `-O3 -march=native` does not recompile linked library objects and show absent LTO only as context. |
| Evidence/status | Direct CMake semantics plus build-system inference. |
| Source evidence | [CMakeLists.txt](../../CMakeLists.txt) |
| Backlog | [BEN-01](../../ENGINEERING_BACKLOG.md#ben-01--make-build-and-benchmark-provenance-reproducible), [INF-02](../../ENGINEERING_BACKLOG.md#inf-02--modernize-target-scoped-cmake-behavior) |
| Findings | [HEL-005](11-current-technical-debt-overlay.md#hel-005), [HEL-047](11-current-technical-debt-overlay.md#hel-047) |
| Related atlas material | — |

```mermaid
flowchart LR
    G["[DEBT] global -Wall -Wextra -O2"]:::debt
    LS["[CURRENT] library sources"]:::current
    LO["[CURRENT] library object files (-O2 plus config flags)"]:::current
    EO["[CURRENT] executable TU (-O3 -march=native)"]:::current
    LK["[CURRENT] linker combines objects"]:::current
    LTO["[CONTEXT — NOT IMPLEMENTED] LTO/IPO recompilation opportunity"]:::context
    G -->|compile options| LS
    LS -->|compile once| LO
    G -->|base options| EO
    EO -->|target-local options added| LK
    LO -->|already compiled| LK
    LTO -.->|absent; no cross-TU optimization claim| LK
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - Target compile options attach to the executable’s own compilation, not to a dependency target’s existing objects.
  - The LTO node is dotted context because CMake does not enable IPO/LTO.
- **What exists:** Global/config flags, target-local executable flags, separate library compilation, and normal linkage.
- **What does not exist:** Evidence that library code used the executable-local flags or that LTO was enabled.
