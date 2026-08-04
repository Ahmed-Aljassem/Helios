# 01 — Current System Context

The solid boundary is intentionally small: historical-file tools, a single-writer in-memory book, verification executables, and evidence artifacts.
<a id="a01-01"></a>
### A01-01 — CURRENT C4-style context

| Diagram card field | Value |
|---|---|
| Purpose and scope | Place users and tracked Helios executables around the current software boundary. |
| Evidence/status | Repository evidence. |
| Source evidence | [CMakeLists.txt](../../CMakeLists.txt), [itch_replay.cpp](../../benchmarks/itch_replay.cpp), [itch_book_replay.cpp](../../benchmarks/itch_book_replay.cpp), [test_orderbook.cpp](../../tests/test_orderbook.cpp) |
| Backlog | [DOC-01](../../ENGINEERING_BACKLOG.md#doc-01--reconcile-architecture-and-evidence-documentation), [VER-06](../../ENGINEERING_BACKLOG.md#ver-06--establish-artifact-and-replay-provenance) |
| Findings | [HEL-007](11-current-technical-debt-overlay.md#hel-007), [HEL-011](11-current-technical-debt-overlay.md#hel-011), [HEL-048](11-current-technical-debt-overlay.md#hel-048) |
| Related atlas material | — |

```mermaid
flowchart LR
    U["[CURRENT] developer / operator"]:::current
    F["[CURRENT] historical BinaryFILE"]:::current
    H["[CURRENT] Helios parser/replay/book boundary"]:::current
    B["[CURRENT] benchmark executables"]:::current
    T["[CURRENT] tests + manual test"]:::current
    A["[DEBT] saved reports/profiles"]:::debt
    U -->|selects file/symbol/workload| H
    F -->|mmap bytes| H
    U -->|runs| B
    U -->|runs| T
    B -->|observes book + emits output| A
    H -->|emits replay output| A
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - The developer controls invocation; the OS owns file backing while the process owns its mapping.
  - Benchmarks and tests call the same core library where linked.
- **What exists:** Historical parser/replay tools, benchmark/test executables, and tracked output artifacts.
- **What does not exist:** An external service boundary, persistent database, daemon, or network session.

<a id="a01-02"></a>
### A01-02 — Explicit out-of-scope ring

| Diagram card field | Value |
|---|---|
| Purpose and scope | Prevent adjacent trading-platform capabilities from being mistaken for Helios features. |
| Evidence/status | Scope declaration; context only. |
| Source evidence | [README.md](../../README.md), [01-project-from-first-principles.md](../../docs/helios-study-guide/01-project-from-first-principles.md) |
| Backlog | [COR-08](../../ENGINEERING_BACKLOG.md#cor-08--separate-reconstruction-and-matching-semantics), [FUT-06](../../ENGINEERING_BACKLOG.md#fut-06--design-real-time-ingestion-boundaries), [FUT-07](../../ENGINEERING_BACKLOG.md#fut-07--decide-whether-helios-will-ever-include-matching-engine-semantics) |
| Findings | [HEL-037](11-current-technical-debt-overlay.md#hel-037), [HEL-042](11-current-technical-debt-overlay.md#hel-042) |
| Related atlas material | — |

```mermaid
flowchart LR
    H["[CURRENT] Helios historical research core"]:::current
    N["[OUT OF SCOPE] live networking/session"]:::context
    O["[OUT OF SCOPE] OMS/strategy/risk/routing"]:::context
    R["[OUT OF SCOPE] recovery/redundant feeds"]:::context
    K["[OUT OF SCOPE] kernel bypass/FPGA"]:::context
    D["[OUT OF SCOPE] distribution/production monitoring"]:::context
    N -.->|not connected| H
    O -.->|not connected| H
    R -.->|not connected| H
    K -.->|research only| H
    D -.->|not connected| H
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - Every dotted edge says not connected, making the ring educational rather than architectural evidence.
- **What exists:** Only the historical-file research core.
- **What does not exist:** Live networking, OMS, strategy, risk, routing, recovery, redundant feeds, kernel bypass, FPGA, distribution, and production monitoring.
