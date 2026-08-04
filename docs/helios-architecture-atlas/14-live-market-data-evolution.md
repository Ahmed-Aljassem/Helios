# 14 — Live Market-Data Evolution

All live components are outside the current book core. Kernel bypass is optional transport research only after correctness, recovery, and observability.
<a id="a14-01"></a>
### A14-01 — Eight-stage live maturity ladder

| Diagram card field | Value |
|---|---|
| Purpose and scope | Sequence live-feed ambition behind correctness, session, recovery, and operational gates. |
| Evidence/status | PROPOSED stages 1–4; later stages are RESEARCH. |
| Source evidence | [itch_parser.hpp](../../include/itch_parser.hpp), [book_replay.hpp](../../include/book_replay.hpp), [13-production-redesign.md](../../docs/helios-study-guide/13-production-redesign.md) |
| Backlog | [PRO-01](../../ENGINEERING_BACKLOG.md#pro-01--introduce-structured-framing-and-decode-outcomes), [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [PRO-06](../../ENGINEERING_BACKLOG.md#pro-06--specify-portable-file-mapping-and-prefault-behavior), [FUT-02](../../ENGINEERING_BACKLOG.md#fut-02--design-multi-symbol-single-writer-sharding), [FUT-06](../../ENGINEERING_BACKLOG.md#fut-06--design-real-time-ingestion-boundaries) |
| Findings | [HEL-026](11-current-technical-debt-overlay.md#hel-026), [HEL-037](11-current-technical-debt-overlay.md#hel-037), [HEL-042](11-current-technical-debt-overlay.md#hel-042) |
| Related atlas material | — |

```mermaid
flowchart TB
    S1["[PROPOSED] 1. checked historical replay"]:::proposed
    S2["[PROPOSED] 2. deterministic session/lifecycle model"]:::proposed
    S3["[PROPOSED] 3. offline packet/message fixture ingestion"]:::proposed
    S4["[PROPOSED] 4. live socket/session receiver"]:::proposed
    S5["[RESEARCH] 5. sequence/gap detection"]:::research
    S6["[RESEARCH] 6. redundant-feed arbitration + recovery"]:::research
    S7["[RESEARCH] 7. bounded routing/publication with backpressure"]:::research
    S8["[RESEARCH] 8. operated multi-symbol service"]:::research
    S1 -->|promotion gate| S2
    S2 -->|promotion gate| S3
    S3 -->|promotion gate| S4
    S4 -->|promotion gate| S5
    S5 -->|promotion gate| S6
    S6 -->|promotion gate| S7
    S7 -->|promotion gate| S8
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - Every stage requires verification evidence from the previous one.
  - Networking does not enter the current core directly.
- **What exists:** Only the historical decoder and single-writer book concepts inform the eight-stage live maturity ladder view today.
- **What does not exist:** The live/recovery/publication components in the eight-stage live maturity ladder view are not implemented.

<a id="a14-02"></a>
### A14-02 — Sequencing, redundancy, and recovery

| Diagram card field | Value |
|---|---|
| Purpose and scope | Keep sequencing, redundancy, and recovery outside the book mutation core with explicit ownership/failure edges. |
| Evidence/status | PROPOSED/RESEARCH/CONTEXT only. |
| Source evidence | [13-production-redesign.md](../../docs/helios-study-guide/13-production-redesign.md), [orderbook.hpp](../../include/orderbook.hpp) |
| Backlog | [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [PRO-05](../../ENGINEERING_BACKLOG.md#pro-05--preserve-diagnostic-feed-metadata), [FUT-02](../../ENGINEERING_BACKLOG.md#fut-02--design-multi-symbol-single-writer-sharding), [FUT-06](../../ENGINEERING_BACKLOG.md#fut-06--design-real-time-ingestion-boundaries) |
| Findings | [HEL-037](11-current-technical-debt-overlay.md#hel-037), [HEL-038](11-current-technical-debt-overlay.md#hel-038), [HEL-042](11-current-technical-debt-overlay.md#hel-042) |
| Related atlas material | — |

```mermaid
flowchart LR
    A["[CONTEXT — NOT IMPLEMENTED] feed A session"]:::context
    B["[CONTEXT — NOT IMPLEMENTED] feed B session"]:::context
    S["[PROPOSED] sequencer/gap detector"]:::proposed
    R["[RESEARCH] arbitration/retransmit/snapshot recovery"]:::research
    N["[PROPOSED] normalized ordered events"]:::proposed
    A -.-> S
    B -.-> S
    S -->|in-order| N
    S -.->|gap/divergence| R
    R -.->|recovered sequence| N
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - A -.-> S
  - B -.-> S
  - S -->|in-order| N
  - S -.->|gap/divergence| R
  - R -.->|recovered sequence| N
- **What exists:** Only the historical decoder and single-writer book concepts inform the sequencing, redundancy, and recovery view today.
- **What does not exist:** The live/recovery/publication components in the sequencing, redundancy, and recovery view are not implemented.

<a id="a14-03"></a>
### A14-03 — Buffering and backpressure

| Diagram card field | Value |
|---|---|
| Purpose and scope | Keep buffering and backpressure outside the book mutation core with explicit ownership/failure edges. |
| Evidence/status | PROPOSED/RESEARCH/CONTEXT only. |
| Source evidence | [13-production-redesign.md](../../docs/helios-study-guide/13-production-redesign.md), [orderbook.hpp](../../include/orderbook.hpp) |
| Backlog | [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [PRO-05](../../ENGINEERING_BACKLOG.md#pro-05--preserve-diagnostic-feed-metadata), [FUT-02](../../ENGINEERING_BACKLOG.md#fut-02--design-multi-symbol-single-writer-sharding), [FUT-06](../../ENGINEERING_BACKLOG.md#fut-06--design-real-time-ingestion-boundaries) |
| Findings | [HEL-037](11-current-technical-debt-overlay.md#hel-037), [HEL-038](11-current-technical-debt-overlay.md#hel-038), [HEL-042](11-current-technical-debt-overlay.md#hel-042) |
| Related atlas material | — |

```mermaid
flowchart LR
    RX["[CONTEXT — NOT IMPLEMENTED] receiver"]:::context
    Q["[PROPOSED] bounded SPSC/shard queue"]:::proposed
    O["[PROPOSED] single-writer owner"]:::proposed
    P["[CONTEXT — NOT IMPLEMENTED] publisher"]:::context
    F["[PROPOSED] explicit overflow policy/health"]:::proposed
    RX -.->|events| Q
    Q -->|ordered dequeue| O
    O -.->|snapshot/delta| P
    Q -->|high-water/overflow| F
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - RX -.->|events| Q
  - Q -->|ordered dequeue| O
  - O -.->|snapshot/delta| P
  - Q -->|high-water/overflow| F
- **What exists:** Only the historical decoder and single-writer book concepts inform the buffering and backpressure view today.
- **What does not exist:** The live/recovery/publication components in the buffering and backpressure view are not implemented.

<a id="a14-04"></a>
### A14-04 — Normalized publication boundary

| Diagram card field | Value |
|---|---|
| Purpose and scope | Keep normalized publication boundary outside the book mutation core with explicit ownership/failure edges. |
| Evidence/status | PROPOSED/RESEARCH/CONTEXT only. |
| Source evidence | [13-production-redesign.md](../../docs/helios-study-guide/13-production-redesign.md), [orderbook.hpp](../../include/orderbook.hpp) |
| Backlog | [PRO-02](../../ENGINEERING_BACKLOG.md#pro-02--validate-session-and-order-lifecycle-integrity), [PRO-05](../../ENGINEERING_BACKLOG.md#pro-05--preserve-diagnostic-feed-metadata), [FUT-02](../../ENGINEERING_BACKLOG.md#fut-02--design-multi-symbol-single-writer-sharding), [FUT-06](../../ENGINEERING_BACKLOG.md#fut-06--design-real-time-ingestion-boundaries) |
| Findings | [HEL-037](11-current-technical-debt-overlay.md#hel-037), [HEL-038](11-current-technical-debt-overlay.md#hel-038), [HEL-042](11-current-technical-debt-overlay.md#hel-042) |
| Related atlas material | — |

```mermaid
flowchart LR
    N["[PROPOSED] normalized lifecycle events"]:::proposed
    B["[PROPOSED] reconstruction book owner"]:::proposed
    S["[PROPOSED] immutable snapshot/delta"]:::proposed
    C["[CONTEXT — NOT IMPLEMENTED] strategy/risk/analytics consumers"]:::context
    K["[RESEARCH] kernel-bypass transport"]:::research
    N --> B
    B --> S
    S -.-> C
    K -.->|optional receiver optimization after proof| N
    classDef current fill:#eef7ee,stroke:#1b5e20,stroke-width:2px,color:#111;
    classDef debt fill:#fff4d6,stroke:#b26a00,stroke-width:2px,color:#111;
    classDef invalid fill:#ffe8e8,stroke:#b71c1c,stroke-width:3px,color:#111;
    classDef proposed fill:#e8f1ff,stroke:#1565c0,stroke-width:2px,stroke-dasharray: 6 4,color:#111;
    classDef research fill:#f3f3f3,stroke:#666,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
    classDef context fill:#fafafa,stroke:#777,stroke-width:2px,stroke-dasharray: 2 4,color:#111;
```

- **Important edges**
  - N --> B
  - B --> S
  - S -.-> C
  - K -.->|optional receiver optimization after proof| N
- **What exists:** Only the historical decoder and single-writer book concepts inform the normalized publication boundary view today.
- **What does not exist:** The live/recovery/publication components in the normalized publication boundary view are not implemented.
