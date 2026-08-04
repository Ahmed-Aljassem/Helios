# 06 — Order Lifecycle State Machines

Red states are reachable invalid behavior or unclassified lifecycle events; dashed blue states are corrective policy, not present code.
<a id="a06-01"></a>
### A06-01 — Generic order

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful generic order state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp), [price_level.hpp](../../include/price_level.hpp), [price_level.cpp](../../src/price_level.cpp), [object_pool.hpp](../../include/object_pool.hpp), [order.hpp](../../include/order.hpp), [itch_parser.hpp](../../include/itch_parser.hpp), [book_replay.hpp](../../include/book_replay.hpp), [itch_replay.cpp](../../benchmarks/itch_replay.cpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-04](../../ENGINEERING_BACKLOG.md#cor-04--separate-reduce-and-quantity-increase-priority-semantics), [COR-06](../../ENGINEERING_BACKLOG.md#cor-06--define-mutation-exception-safety-guarantees), [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [FUT-01](../../ENGINEERING_BACKLOG.md#fut-01--reassess-the-generic-objectpoolt-contract) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-004](11-current-technical-debt-overlay.md#hel-004), [HEL-012](11-current-technical-debt-overlay.md#hel-012), [HEL-013](11-current-technical-debt-overlay.md#hel-013), [HEL-021](11-current-technical-debt-overlay.md#hel-021), [HEL-037](11-current-technical-debt-overlay.md#hel-037) |
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
- **What exists:** Solid CURRENT transitions are reachable in present code.
- **What does not exist:** Proposed policy transitions are not implemented; red transitions are not validated into explicit outcomes.

<a id="a06-02"></a>
### A06-02 — ITCH reference

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful itch reference state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp), [price_level.hpp](../../include/price_level.hpp), [price_level.cpp](../../src/price_level.cpp), [object_pool.hpp](../../include/object_pool.hpp), [order.hpp](../../include/order.hpp), [itch_parser.hpp](../../include/itch_parser.hpp), [book_replay.hpp](../../include/book_replay.hpp), [itch_replay.cpp](../../benchmarks/itch_replay.cpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-04](../../ENGINEERING_BACKLOG.md#cor-04--separate-reduce-and-quantity-increase-priority-semantics), [COR-06](../../ENGINEERING_BACKLOG.md#cor-06--define-mutation-exception-safety-guarantees), [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [FUT-01](../../ENGINEERING_BACKLOG.md#fut-01--reassess-the-generic-objectpoolt-contract) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-004](11-current-technical-debt-overlay.md#hel-004), [HEL-012](11-current-technical-debt-overlay.md#hel-012), [HEL-013](11-current-technical-debt-overlay.md#hel-013), [HEL-021](11-current-technical-debt-overlay.md#hel-021), [HEL-037](11-current-technical-debt-overlay.md#hel-037) |
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
- **What exists:** Solid CURRENT transitions are reachable in present code.
- **What does not exist:** Proposed policy transitions are not implemented; red transitions are not validated into explicit outcomes.

<a id="a06-03"></a>
### A06-03 — Pool slot

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful pool slot state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp), [price_level.hpp](../../include/price_level.hpp), [price_level.cpp](../../src/price_level.cpp), [object_pool.hpp](../../include/object_pool.hpp), [order.hpp](../../include/order.hpp), [itch_parser.hpp](../../include/itch_parser.hpp), [book_replay.hpp](../../include/book_replay.hpp), [itch_replay.cpp](../../benchmarks/itch_replay.cpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-04](../../ENGINEERING_BACKLOG.md#cor-04--separate-reduce-and-quantity-increase-priority-semantics), [COR-06](../../ENGINEERING_BACKLOG.md#cor-06--define-mutation-exception-safety-guarantees), [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [FUT-01](../../ENGINEERING_BACKLOG.md#fut-01--reassess-the-generic-objectpoolt-contract) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-004](11-current-technical-debt-overlay.md#hel-004), [HEL-012](11-current-technical-debt-overlay.md#hel-012), [HEL-013](11-current-technical-debt-overlay.md#hel-013), [HEL-021](11-current-technical-debt-overlay.md#hel-021), [HEL-037](11-current-technical-debt-overlay.md#hel-037) |
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
- **What exists:** Solid CURRENT transitions are reachable in present code.
- **What does not exist:** Proposed policy transitions are not implemented; red transitions are not validated into explicit outcomes.

<a id="a06-04"></a>
### A06-04 — Price level

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful price level state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp), [price_level.hpp](../../include/price_level.hpp), [price_level.cpp](../../src/price_level.cpp), [object_pool.hpp](../../include/object_pool.hpp), [order.hpp](../../include/order.hpp), [itch_parser.hpp](../../include/itch_parser.hpp), [book_replay.hpp](../../include/book_replay.hpp), [itch_replay.cpp](../../benchmarks/itch_replay.cpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-04](../../ENGINEERING_BACKLOG.md#cor-04--separate-reduce-and-quantity-increase-priority-semantics), [COR-06](../../ENGINEERING_BACKLOG.md#cor-06--define-mutation-exception-safety-guarantees), [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [FUT-01](../../ENGINEERING_BACKLOG.md#fut-01--reassess-the-generic-objectpoolt-contract) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-004](11-current-technical-debt-overlay.md#hel-004), [HEL-012](11-current-technical-debt-overlay.md#hel-012), [HEL-013](11-current-technical-debt-overlay.md#hel-013), [HEL-021](11-current-technical-debt-overlay.md#hel-021), [HEL-037](11-current-technical-debt-overlay.md#hel-037) |
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
- **What exists:** Solid CURRENT transitions are reachable in present code.
- **What does not exist:** Proposed policy transitions are not implemented; red transitions are not validated into explicit outcomes.

<a id="a06-05"></a>
### A06-05 — Bitmap bit

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful bitmap bit state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp), [price_level.hpp](../../include/price_level.hpp), [price_level.cpp](../../src/price_level.cpp), [object_pool.hpp](../../include/object_pool.hpp), [order.hpp](../../include/order.hpp), [itch_parser.hpp](../../include/itch_parser.hpp), [book_replay.hpp](../../include/book_replay.hpp), [itch_replay.cpp](../../benchmarks/itch_replay.cpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-04](../../ENGINEERING_BACKLOG.md#cor-04--separate-reduce-and-quantity-increase-priority-semantics), [COR-06](../../ENGINEERING_BACKLOG.md#cor-06--define-mutation-exception-safety-guarantees), [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [FUT-01](../../ENGINEERING_BACKLOG.md#fut-01--reassess-the-generic-objectpoolt-contract) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-004](11-current-technical-debt-overlay.md#hel-004), [HEL-012](11-current-technical-debt-overlay.md#hel-012), [HEL-013](11-current-technical-debt-overlay.md#hel-013), [HEL-021](11-current-technical-debt-overlay.md#hel-021), [HEL-037](11-current-technical-debt-overlay.md#hel-037) |
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
- **What exists:** Solid CURRENT transitions are reachable in present code.
- **What does not exist:** Proposed policy transitions are not implemented; red transitions are not validated into explicit outcomes.

<a id="a06-06"></a>
### A06-06 — Cached best

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful cached best state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp), [price_level.hpp](../../include/price_level.hpp), [price_level.cpp](../../src/price_level.cpp), [object_pool.hpp](../../include/object_pool.hpp), [order.hpp](../../include/order.hpp), [itch_parser.hpp](../../include/itch_parser.hpp), [book_replay.hpp](../../include/book_replay.hpp), [itch_replay.cpp](../../benchmarks/itch_replay.cpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-04](../../ENGINEERING_BACKLOG.md#cor-04--separate-reduce-and-quantity-increase-priority-semantics), [COR-06](../../ENGINEERING_BACKLOG.md#cor-06--define-mutation-exception-safety-guarantees), [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [FUT-01](../../ENGINEERING_BACKLOG.md#fut-01--reassess-the-generic-objectpoolt-contract) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-004](11-current-technical-debt-overlay.md#hel-004), [HEL-012](11-current-technical-debt-overlay.md#hel-012), [HEL-013](11-current-technical-debt-overlay.md#hel-013), [HEL-021](11-current-technical-debt-overlay.md#hel-021), [HEL-037](11-current-technical-debt-overlay.md#hel-037) |
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
- **What exists:** Solid CURRENT transitions are reachable in present code.
- **What does not exist:** Proposed policy transitions are not implemented; red transitions are not validated into explicit outcomes.

<a id="a06-07"></a>
### A06-07 — Decoded message

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful decoded message state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp), [price_level.hpp](../../include/price_level.hpp), [price_level.cpp](../../src/price_level.cpp), [object_pool.hpp](../../include/object_pool.hpp), [order.hpp](../../include/order.hpp), [itch_parser.hpp](../../include/itch_parser.hpp), [book_replay.hpp](../../include/book_replay.hpp), [itch_replay.cpp](../../benchmarks/itch_replay.cpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-04](../../ENGINEERING_BACKLOG.md#cor-04--separate-reduce-and-quantity-increase-priority-semantics), [COR-06](../../ENGINEERING_BACKLOG.md#cor-06--define-mutation-exception-safety-guarantees), [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [FUT-01](../../ENGINEERING_BACKLOG.md#fut-01--reassess-the-generic-objectpoolt-contract) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-004](11-current-technical-debt-overlay.md#hel-004), [HEL-012](11-current-technical-debt-overlay.md#hel-012), [HEL-013](11-current-technical-debt-overlay.md#hel-013), [HEL-021](11-current-technical-debt-overlay.md#hel-021), [HEL-037](11-current-technical-debt-overlay.md#hel-037) |
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
- **What exists:** Solid CURRENT transitions are reachable in present code.
- **What does not exist:** Proposed policy transitions are not implemented; red transitions are not validated into explicit outcomes.

<a id="a06-08"></a>
### A06-08 — Mapped file

| Diagram card field | Value |
|---|---|
| Purpose and scope | Model every meaningful mapped file state and transition, including invalid transitions. |
| Evidence/status | Repository-derived state machine with audit-labeled invalid states. |
| Source evidence | [orderbook.hpp](../../include/orderbook.hpp), [orderbook.cpp](../../src/orderbook.cpp), [price_level.hpp](../../include/price_level.hpp), [price_level.cpp](../../src/price_level.cpp), [object_pool.hpp](../../include/object_pool.hpp), [order.hpp](../../include/order.hpp), [itch_parser.hpp](../../include/itch_parser.hpp), [book_replay.hpp](../../include/book_replay.hpp), [itch_replay.cpp](../../benchmarks/itch_replay.cpp) |
| Backlog | [COR-02](../../ENGINEERING_BACKLOG.md#cor-02--define-order-id-uniqueness-and-namespace-policy), [COR-03](../../ENGINEERING_BACKLOG.md#cor-03--define-zero-quantity-lifecycle-semantics), [COR-04](../../ENGINEERING_BACKLOG.md#cor-04--separate-reduce-and-quantity-increase-priority-semantics), [COR-06](../../ENGINEERING_BACKLOG.md#cor-06--define-mutation-exception-safety-guarantees), [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [FUT-01](../../ENGINEERING_BACKLOG.md#fut-01--reassess-the-generic-objectpoolt-contract) |
| Findings | [HEL-002](11-current-technical-debt-overlay.md#hel-002), [HEL-003](11-current-technical-debt-overlay.md#hel-003), [HEL-004](11-current-technical-debt-overlay.md#hel-004), [HEL-012](11-current-technical-debt-overlay.md#hel-012), [HEL-013](11-current-technical-debt-overlay.md#hel-013), [HEL-021](11-current-technical-debt-overlay.md#hel-021), [HEL-037](11-current-technical-debt-overlay.md#hel-037) |
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
- **What exists:** Solid CURRENT transitions are reachable in present code.
- **What does not exist:** Proposed policy transitions are not implemented; red transitions are not validated into explicit outcomes.
