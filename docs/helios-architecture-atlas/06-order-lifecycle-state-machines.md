# 06 — Order Lifecycle State Machines

Red states are reachable invalid behavior or unclassified lifecycle events; dashed blue states are corrective policy, not present code.
<a id="a06-01"></a>
### A06-01 — Generic order

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful generic order state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp), [price_level.hpp](../../include/price_level.hpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-04](../../ENGINEERING_BACKLOG.md#cor-04--separate-reduce-and-quantity-increase-priority-semantics), [VER-02](../../ENGINEERING_BACKLOG.md#ver-02--build-a-reference-book-and-differential-invariant-harness) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-004](11-current-technical-debt-overlay.md#hel-004), [HEL-009](11-current-technical-debt-overlay.md#hel-009) |
| Related atlas material | — |

```mermaid
flowchart LR
    N["[CURRENT] nonexistent"]:::current
    L["[CURRENT] live/indexed/queued"]:::current
    Q["[DEBT] live with zero quantity"]:::debt
    R["[CURRENT] removed/free"]:::current
    D["[INVALID] duplicate ID orphan"]:::invalid
    P["[PROPOSED] reject/transaction policy"]:::proposed
    N -->|valid add| L
    L -->|reduce to positive| L
    L -->|modify to zero| Q
    L -->|cancel/full fill| R
    L -->|duplicate add| D
    D -.->|COR-02| P
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - N -->|valid add| L
  - L -->|reduce to positive| L
  - L -->|modify to zero| Q
  - L -->|cancel/full fill| R
  - L -->|duplicate add| D
  - D -.->|COR-02| P
- **What exists:** The CURRENT/DEBT transitions in the generic order machine are reachable or directly derived from present code.
- **What does not exist:** PROPOSED transitions in the generic order machine are unimplemented; INVALID states remain defects or unclassified outcomes.

<a id="a06-02"></a>
### A06-02 — ITCH reference

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful itch reference state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [book_replay.hpp](../../include/book_replay.hpp), [orderbook.cpp](../../src/orderbook.cpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-037](11-current-technical-debt-overlay.md#hel-037) |
| Related atlas material | — |

```mermaid
flowchart LR
    U["[CURRENT] unseen reference"]:::current
    L["[CURRENT] target-symbol live"]:::current
    R["[CURRENT] removed"]:::current
    B["[INVALID] update before add"]:::invalid
    A["[INVALID] update after removal"]:::invalid
    D["[INVALID] duplicate live reference"]:::invalid
    U -->|A/F target| L
    L -->|X/E/C partial| L
    L -->|D/full X/E/C/U-old| R
    U -->|D/X/E/C/U| B
    R -->|D/X/E/C/U| A
    L -->|A/F same ref| D
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - U -->|A/F target| L
  - L -->|X/E/C partial| L
  - L -->|D/full X/E/C/U-old| R
  - U -->|D/X/E/C/U| B
  - R -->|D/X/E/C/U| A
  - L -->|A/F same ref| D
- **What exists:** The CURRENT/DEBT transitions in the itch reference machine are reachable or directly derived from present code.
- **What does not exist:** PROPOSED transitions in the itch reference machine are unimplemented; INVALID states remain defects or unclassified outcomes.

<a id="a06-03"></a>
### A06-03 — Pool slot

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful pool slot state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [object_pool.hpp](../../include/object_pool.hpp) |
| Backlog | [COR-07](../../ENGINEERING_BACKLOG.md#cor-07--define-capacity-and-growth-policy), [FUT-01](../../ENGINEERING_BACKLOG.md#fut-01--reassess-the-generic-objectpoolt-contract) |
| Findings | [HEL-013](11-current-technical-debt-overlay.md#hel-013), [HEL-035](11-current-technical-debt-overlay.md#hel-035) |
| Related atlas material | — |

```mermaid
flowchart LR
    F["[CURRENT] free-list slot"]:::current
    C["[CURRENT] constructed live Order"]:::current
    R["[CURRENT] destructed/returned"]:::current
    G["[DEBT] grow chunk"]:::debt
    X["[INVALID] live non-trivial object at pool destruction"]:::invalid
    F -->|placement construct| C
    C -->|explicit destructor + push| R
    R -->|available as free| F
    F -->|free list empty| G
    G -->|new slots| F
    C -->|pool destroyed| X
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - F -->|placement construct| C
  - C -->|explicit destructor + push| R
  - R -->|available as free| F
  - F -->|free list empty| G
  - G -->|new slots| F
  - C -->|pool destroyed| X
- **What exists:** The CURRENT/DEBT transitions in the pool slot machine are reachable or directly derived from present code.
- **What does not exist:** PROPOSED transitions in the pool slot machine are unimplemented; INVALID states remain defects or unclassified outcomes.

<a id="a06-04"></a>
### A06-04 — Price level

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful price level state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [price_level.hpp](../../include/price_level.hpp), [price_level.cpp](../../src/price_level.cpp), [orderbook.cpp](../../src/orderbook.cpp) |
| Backlog | [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-05](../../ENGINEERING_BACKLOG.md#cor-05--harden-numeric-and-construction-boundaries), [VER-02](../../ENGINEERING_BACKLOG.md#ver-02--build-a-reference-book-and-differential-invariant-harness) |
| Findings | [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-009](11-current-technical-debt-overlay.md#hel-009), [HEL-020](11-current-technical-debt-overlay.md#hel-020) |
| Related atlas material | — |

```mermaid
flowchart LR
    E["[CURRENT] empty total=0 count=0"]:::current
    O["[CURRENT] occupied FIFO/aggregate"]:::current
    Z["[DEBT] occupied zero-liquidity"]:::debt
    X["[INVALID] aggregate/count/topology mismatch"]:::invalid
    E -->|first append| O
    O -->|append/remove/positive modify| O
    O -->|remove last| E
    E -->|zero-quantity add| Z
    O -->|unchecked underflow/bad membership| X
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - E -->|first append| O
  - O -->|append/remove/positive modify| O
  - O -->|remove last| E
  - E -->|zero-quantity add| Z
  - O -->|unchecked underflow/bad membership| X
- **What exists:** The CURRENT/DEBT transitions in the price level machine are reachable or directly derived from present code.
- **What does not exist:** PROPOSED transitions in the price level machine are unimplemented; INVALID states remain defects or unclassified outcomes.

<a id="a06-05"></a>
### A06-05 — Bitmap bit

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful bitmap bit state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp) |
| Backlog | [VER-02](../../ENGINEERING_BACKLOG.md#ver-02--build-a-reference-book-and-differential-invariant-harness), [BEN-09](../../ENGINEERING_BACKLOG.md#ben-09--measure-bitmap-refresh-complexity-and-alternatives) |
| Findings | [HEL-009](11-current-technical-debt-overlay.md#hel-009), [HEL-027](11-current-technical-debt-overlay.md#hel-027) |
| Related atlas material | — |

```mermaid
flowchart LR
    C["[CURRENT] clear"]:::current
    S["[CURRENT] set"]:::current
    X["[INVALID] disagrees with level emptiness"]:::invalid
    C -->|first order at level| S
    S -->|last order removed| C
    C -->|occupied level without set| X
    S -->|empty level without clear| X
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - C -->|first order at level| S
  - S -->|last order removed| C
  - C -->|occupied level without set| X
  - S -->|empty level without clear| X
- **What exists:** The CURRENT/DEBT transitions in the bitmap bit machine are reachable or directly derived from present code.
- **What does not exist:** PROPOSED transitions in the bitmap bit machine are unimplemented; INVALID states remain defects or unclassified outcomes.

<a id="a06-06"></a>
### A06-06 — Cached best

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful cached best state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp) |
| Backlog | [VER-02](../../ENGINEERING_BACKLOG.md#ver-02--build-a-reference-book-and-differential-invariant-harness), [BEN-09](../../ENGINEERING_BACKLOG.md#ben-09--measure-bitmap-refresh-complexity-and-alternatives) |
| Findings | [HEL-009](11-current-technical-debt-overlay.md#hel-009), [HEL-027](11-current-technical-debt-overlay.md#hel-027) |
| Related atlas material | — |

```mermaid
flowchart LR
    N["[CURRENT] -1/no side liquidity"]:::current
    B["[CURRENT] index of best occupied level"]:::current
    R["[CURRENT] bitmap rescan result"]:::current
    X["[INVALID] stale/unoccupied index"]:::invalid
    N -->|first level| B
    B -->|better level added| B
    B -->|best level emptied| R
    R -->|found| B
    R -->|none| N
    B -->|missed bitmap/cache update| X
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - N -->|first level| B
  - B -->|better level added| B
  - B -->|best level emptied| R
  - R -->|found| B
  - R -->|none| N
  - B -->|missed bitmap/cache update| X
- **What exists:** The CURRENT/DEBT transitions in the cached best machine are reachable or directly derived from present code.
- **What does not exist:** PROPOSED transitions in the cached best machine are unimplemented; INVALID states remain defects or unclassified outcomes.

<a id="a06-07"></a>
### A06-07 — Decoded message

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful decoded message state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [itch_parser.hpp](../../include/itch_parser.hpp) |
| Backlog | [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [VER-01](../../ENGINEERING_BACKLOG.md#ver-01--build-protocol-golden-fixtures), [VER-05](../../ENGINEERING_BACKLOG.md#ver-05--add-sanitizer-fuzzing-and-boundary-verification-strategy) |
| Findings | [HEL-011](11-current-technical-debt-overlay.md#hel-011), [HEL-012](11-current-technical-debt-overlay.md#hel-012), [HEL-022](11-current-technical-debt-overlay.md#hel-022) |
| Related atlas material | — |

```mermaid
flowchart LR
    E["[CURRENT] empty/default Message"]:::current
    M["[CURRENT] modeled decoded"]:::current
    U["[DEBT] unsupported decoded as false"]:::debt
    F["[INVALID] malformed modeled decoded as same false"]:::invalid
    C["[CURRENT] callback consumed synchronously"]:::current
    P["[PROPOSED] structured outcome"]:::proposed
    E -->|exact/minimum fields available| M
    E -->|unknown type| U
    E -->|short modeled body| F
    M -->|callback| C
    U -.->|PRO-01| P
    F -.->|PRO-01| P
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - E -->|exact/minimum fields available| M
  - E -->|unknown type| U
  - E -->|short modeled body| F
  - M -->|callback| C
  - U -.->|PRO-01| P
  - F -.->|PRO-01| P
- **What exists:** The CURRENT/DEBT transitions in the decoded message machine are reachable or directly derived from present code.
- **What does not exist:** PROPOSED transitions in the decoded message machine are unimplemented; INVALID states remain defects or unclassified outcomes.

<a id="a06-08"></a>
### A06-08 — Mapped file

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful mapped file state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [itch_replay.cpp](../../benchmarks/itch_replay.cpp), [itch_book_replay.cpp](../../benchmarks/itch_book_replay.cpp) |
| Backlog | [PRO-06](../../ENGINEERING_BACKLOG.md#pro-06--specify-portable-file-mapping-and-prefault-behavior) |
| Findings | [HEL-026](11-current-technical-debt-overlay.md#hel-026) |
| Related atlas material | — |

```mermaid
flowchart LR
    N["[CURRENT] no mapping"]:::current
    FD["[CURRENT] open fd + stat"]:::current
    M["[CURRENT] MAP_PRIVATE bytes"]:::current
    P["[DEBT] advice/touch attempted"]:::debt
    U["[CURRENT] unmapped + closed"]:::current
    X["[INVALID] unchecked syscall/empty-file boundary"]:::invalid
    N -->|open/fstat| FD
    FD -->|mmap success| M
    M -->|madvise/touch| P
    P -->|parse then munmap/close| U
    N -->|unchecked args/open in itch_replay| X
    FD -->|unchecked fstat/empty mapping| X
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - N -->|open/fstat| FD
  - FD -->|mmap success| M
  - M -->|madvise/touch| P
  - P -->|parse then munmap/close| U
  - N -->|unchecked args/open in itch_replay| X
  - FD -->|unchecked fstat/empty mapping| X
- **What exists:** The CURRENT/DEBT transitions in the mapped file machine are reachable or directly derived from present code.
- **What does not exist:** PROPOSED transitions in the mapped file machine are unimplemented; INVALID states remain defects or unclassified outcomes.
