# Helios Engineering Backlog

## Purpose

This backlog converts the principal-level Helios audit into a long-term engineering apprenticeship plan. It is intentionally ordered around correctness, experimental rigor, and systems learning rather than feature count or shortest delivery time.

This document plans work only. It does not authorize or contain implementation changes.

## Working Conventions

### Priority

| Priority | Meaning |
|---|---|
| **P0** | Blocks trustworthy order-book or ITCH behavior. Resolve before relying on replay results or latency claims. |
| **P1** | Required to establish strong correctness and verification foundations. |
| **P2** | Required to make performance and operational claims reproducible and defensible. |
| **P3** | Improves maintainability, clarity, portability, and recruiting presentation. |
| **P4** | Future architecture or optimization work that should wait for the evidence foundation. |

### Difficulty

| Size | Expected effort | Typical character |
|---|---|---|
| **XS** | Less than 1 focused day | Isolated documentation or configuration correction |
| **S** | 1–3 days | Narrow behavior with clear boundaries |
| **M** | 3–7 days | Cross-file behavior or a focused verification project |
| **L** | 1–2 weeks | Subsystem redesign or substantial experimental campaign |
| **XL** | 2–6 weeks | Architectural work with migration and broad validation |

### Recruiting impact

| Level | Meaning |
|---|---|
| **Very High** | Likely to become a central interview discussion and materially improve repository credibility |
| **High** | Strong evidence of professional systems-engineering practice |
| **Medium** | Helpful supporting signal or repository polish |
| **Low** | Valuable engineering work but weak near-term recruiting differentiation |

### Journal template codes

| Code | Template |
|---|---|
| **ADR** | Architecture/decision record |
| **INV** | Investigation and root-cause record |
| **VER** | Verification and correctness study |
| **BENCH** | Benchmark/profiling experiment |
| **INFRA** | Build, CI, or operational change record |
| **DOC** | Documentation evidence and retrospective |
| **RFC** | Future architecture proposal |

---

# Epic 1 — Correctness

## COR-01 — Establish a lossless price-domain model

- **Priority:** P0
- **Audit findings:** HEL-001, HEL-017
- **Difficulty:** L
- **Dependencies:** None; this defines contracts used by most later work.
- **Outcome:** One explicit internal price scale and tick-domain policy that can represent every supported ITCH price without collisions or silent range rejection.
- **Acceptance planning:** Define raw feed scale, book tick scale, range behavior, display conversion, instrument-specific tick policy, and migration effects before implementation.
- **Recruiting impact:** **Very High** — demonstrates protocol correctness, type design, and willingness to invalidate an attractive optimization when its domain assumptions are wrong.
- **Journal:** `docs/engineering-journal/01-correctness/01-ITCH-Price-Representation.md`
- **Template:** ADR

## COR-02 — Define order-ID uniqueness and namespace policy

- **Priority:** P0
- **Audit findings:** HEL-002
- **Difficulty:** M
- **Dependencies:** None
- **Outcome:** Explicit behavior for duplicate external IDs, generated IDs, mixed ID sources, and replacement IDs.
- **Acceptance planning:** No insertion may orphan an indexed or queued order; hash size, queued-order count, and global count must remain consistent.
- **Recruiting impact:** **Very High** — creates a strong state-integrity and API-contract story.
- **Journal:** `docs/engineering-journal/01-correctness/02-Order-ID-Uniqueness.md`
- **Template:** ADR

## COR-03 — Define zero-quantity lifecycle semantics

- **Priority:** P0
- **Audit findings:** HEL-003
- **Difficulty:** S
- **Dependencies:** COR-02
- **Outcome:** A single documented policy for zero-share add, reduce, modify, replace, and execution events.
- **Acceptance planning:** The best level must always permit progress or be empty; no zero-liquidity level may remain occupied accidentally.
- **Recruiting impact:** **High** — demonstrates adversarial invariant analysis rather than happy-path testing.
- **Journal:** `docs/engineering-journal/01-correctness/03-Zero-Quantity-Semantics.md`
- **Template:** ADR

## COR-04 — Separate reduce and quantity-increase priority semantics

- **Priority:** P0
- **Audit findings:** HEL-004
- **Difficulty:** M
- **Dependencies:** COR-02, COR-03
- **Outcome:** Distinct and explicit policies for quantity reduction, quantity increase, and cancel-replace.
- **Acceptance planning:** FIFO effects must be documented and verified for every mutation type.
- **Recruiting impact:** **Very High** — directly tests exchange-domain understanding and API design.
- **Journal:** `docs/engineering-journal/01-correctness/04-Modify-and-Priority-Semantics.md`
- **Template:** ADR

## COR-05 — Harden numeric and construction boundaries

- **Priority:** P1
- **Audit findings:** HEL-018, HEL-019, HEL-020
- **Difficulty:** M
- **Dependencies:** COR-01, COR-03
- **Outcome:** Explicit invalid-range handling, documented decimal conversion/rounding, and overflow-safe quantity aggregation.
- **Acceptance planning:** Cover reversed price ranges, boundary prices, maximum quantities, signed/unsigned conversion, aggregate underflow, and decimal conversion behavior.
- **Recruiting impact:** **High** — shows practical C++ integer and API-boundary discipline.
- **Journal:** `docs/engineering-journal/01-correctness/05-Numeric-Boundaries-and-Construction.md`
- **Template:** VER

## COR-06 — Define mutation exception-safety guarantees

- **Priority:** P1
- **Audit findings:** HEL-021
- **Difficulty:** M
- **Dependencies:** COR-02; VER-02 should provide the invariant oracle.
- **Outcome:** A documented basic or strong exception guarantee for pool allocation, hash insertion, queue insertion, and rollback.
- **Acceptance planning:** Inject or simulate allocation failure at every ownership transition and verify invariants.
- **Recruiting impact:** **High** — strong C++ ownership and failure-mode signal.
- **Journal:** `docs/engineering-journal/01-correctness/06-Mutation-Exception-Safety.md`
- **Template:** INV

## COR-07 — Define capacity and growth policy

- **Priority:** P1
- **Audit findings:** HEL-035
- **Difficulty:** M
- **Dependencies:** BEN-02, COR-06
- **Outcome:** Deliberate policy for pool growth, hash rehashing, fixed capacity, failure, and observability.
- **Acceptance planning:** Identify every allocation-capable slow path and specify what happens at capacity.
- **Recruiting impact:** **High** — turns vague “preallocation” into a production-grade capacity contract.
- **Journal:** `docs/engineering-journal/01-correctness/07-Capacity-and-Growth-Policy.md`
- **Template:** ADR

## COR-08 — Separate reconstruction and matching semantics

- **Priority:** P3
- **Audit findings:** HEL-042
- **Difficulty:** M
- **Dependencies:** COR-04
- **Outcome:** A crisp architectural boundary between displayed-book reconstruction and synthetic or exchange-style matching.
- **Acceptance planning:** Every public operation must belong to one semantic model; crossed-book behavior must be intentional.
- **Recruiting impact:** **High** — clarifies what Helios actually is and prevents interview confusion.
- **Journal:** `docs/engineering-journal/01-correctness/08-Reconstruction-vs-Matching.md`
- **Template:** ADR

## COR-09 — Resolve ambiguous and no-op public APIs

- **Priority:** P3
- **Audit findings:** HEL-039, HEL-041
- **Difficulty:** S
- **Dependencies:** COR-08
- **Outcome:** Decide the fate and contract of `printBook`, active-level counters, and names that imply active orders.
- **Acceptance planning:** Public APIs must either work, be removed, or be explicitly marked outside the core library.
- **Recruiting impact:** **Medium** — improves API credibility and repository polish.
- **Journal:** `docs/engineering-journal/01-correctness/09-Public-API-Clarity.md`
- **Template:** ADR

---

# Epic 2 — Protocol Compliance

## PRO-01 — Introduce structured framing and decode outcomes

- **Priority:** P1
- **Audit findings:** HEL-012, HEL-022, HEL-025
- **Difficulty:** L
- **Dependencies:** COR-01
- **Outcome:** Distinguish success, unsupported type, malformed modeled message, truncated tail, incomplete file, terminator, invalid field, and system-call failure.
- **Acceptance planning:** Return type, error offset, message type, consumed bytes, and CLI exit behavior must be specified before implementation.
- **Recruiting impact:** **Very High** — demonstrates robust binary-protocol engineering.
- **Journal:** `docs/engineering-journal/02-protocol/10-Parser-Outcome-Model.md`
- **Template:** ADR

## PRO-02 — Validate session and order-lifecycle integrity

- **Priority:** P1
- **Audit findings:** HEL-037
- **Difficulty:** L
- **Dependencies:** PRO-01, VER-01, VER-02
- **Outcome:** Detect missing terminators, incomplete sessions, duplicate references, update-before-add, update-after-delete, replace anomalies, and unexpected final state.
- **Acceptance planning:** Define fatal, recoverable, counted, and ignored anomaly classes.
- **Recruiting impact:** **Very High** — differentiates a parser demo from a serious feed-replay engine.
- **Journal:** `docs/engineering-journal/02-protocol/11-Session-and-Lifecycle-Integrity.md`
- **Template:** VER

## PRO-03 — Evaluate stock-locate-based symbol routing

- **Priority:** P2
- **Audit findings:** HEL-024
- **Difficulty:** M
- **Dependencies:** PRO-01; COR-01 for instrument metadata design.
- **Outcome:** Decide whether to route by daily stock locate, raw eight-byte symbol, or a two-stage directory lookup.
- **Acceptance planning:** Include daily locate lifecycle, symbol-directory parsing, unknown locate behavior, and multi-symbol implications.
- **Recruiting impact:** **High** — shows use of protocol-native indexing rather than isolated micro-optimization.
- **Journal:** `docs/engineering-journal/02-protocol/12-Stock-Locate-Routing.md`
- **Template:** ADR

## PRO-04 — Define message-count and throughput semantics

- **Priority:** P2
- **Audit findings:** HEL-023
- **Difficulty:** S
- **Dependencies:** PRO-01, BEN-03
- **Outcome:** Separate framed messages, decoded modeled messages, target-symbol events, accepted book mutations, and rejected/anomalous events.
- **Acceptance planning:** Every reported rate must name its numerator and timed boundary.
- **Recruiting impact:** **High** — improves measurement honesty and feed-handler observability.
- **Journal:** `docs/engineering-journal/02-protocol/13-Replay-Metrics-Semantics.md`
- **Template:** ADR

## PRO-05 — Preserve diagnostic feed metadata

- **Priority:** P2
- **Audit findings:** HEL-038
- **Difficulty:** M
- **Dependencies:** PRO-01, PRO-02
- **Outcome:** Decide which timestamp, stock-locate, tracking, and source-position fields must survive decoding and mutation.
- **Acceptance planning:** Separate hot-path state from audit/diagnostic metadata and measure any cost.
- **Recruiting impact:** **Medium** — strengthens debuggability and replay validation.
- **Journal:** `docs/engineering-journal/02-protocol/14-Feed-Metadata-Lifecycle.md`
- **Template:** ADR

## PRO-06 — Specify portable file-mapping and prefault behavior

- **Priority:** P2
- **Audit findings:** HEL-026
- **Difficulty:** M
- **Dependencies:** PRO-01
- **Outcome:** Explicit Linux/macOS behavior for mapping, page-size discovery, advice failures, prefault guarantees, empty files, and cleanup.
- **Acceptance planning:** Separate correctness from performance hints; ignored advice must never be reported as guaranteed prefaulting.
- **Recruiting impact:** **High** — good operating-system and portability signal.
- **Journal:** `docs/engineering-journal/02-protocol/15-Mmap-and-Prefault-Contract.md`
- **Template:** INV

---

# Epic 3 — Verification

## VER-01 — Build protocol golden fixtures

- **Priority:** P1
- **Audit findings:** HEL-011
- **Difficulty:** L
- **Dependencies:** COR-01, PRO-01
- **Outcome:** Minimal authoritative fixtures for every modeled message, framing termination, truncation, invalid fields, and boundary values.
- **Acceptance planning:** Record fixture provenance and expected decoded fields without depending on implementation internals.
- **Recruiting impact:** **Very High** — creates concrete evidence of protocol competence.
- **Journal:** `docs/engineering-journal/03-verification/16-ITCH-Golden-Fixtures.md`
- **Template:** VER

## VER-02 — Build a reference book and differential invariant harness

- **Priority:** P1
- **Audit findings:** HEL-009, HEL-011
- **Difficulty:** L
- **Dependencies:** COR-01 through COR-05
- **Outcome:** A deliberately simple reference representation checked after every generated operation.
- **Acceptance planning:** Verify ID/queue bijection, FIFO, level totals, bitmaps, best prices, counts, and failure behavior.
- **Recruiting impact:** **Very High** — one of the strongest possible signals for rigorous systems engineering.
- **Journal:** `docs/engineering-journal/03-verification/17-Differential-Book-Verification.md`
- **Template:** VER

## VER-03 — Replace the invalid allocation check with a trustworthy methodology

- **Priority:** P1
- **Audit findings:** HEL-008
- **Difficulty:** M
- **Dependencies:** BEN-02, COR-07
- **Outcome:** Measure every relevant allocation family under correct in-range operations and known capacity states.
- **Acceptance planning:** Account for aligned allocation, arrays, hash nodes, rehash, pool chunks, and test instrumentation effects.
- **Recruiting impact:** **High** — demonstrates that allocator claims are measured rather than assumed.
- **Journal:** `docs/engineering-journal/03-verification/18-Allocation-Verification.md`
- **Template:** VER

## VER-04 — Repair workload lifecycle accounting

- **Priority:** P1
- **Audit findings:** HEL-034
- **Difficulty:** M
- **Dependencies:** VER-02
- **Outcome:** Workload generators must not retain IDs removed by executions or replacements unless stale-reference behavior is intentional.
- **Acceptance planning:** Measure requested versus successful operation distribution and final live-set consistency.
- **Recruiting impact:** **High** — shows careful experimental workload construction.
- **Journal:** `docs/engineering-journal/03-verification/19-Workload-Lifecycle-Accounting.md`
- **Template:** VER

## VER-05 — Add sanitizer, fuzzing, and boundary verification strategy

- **Priority:** P2
- **Audit findings:** HEL-011, HEL-022, HEL-046
- **Difficulty:** L
- **Dependencies:** INF-01, PRO-01, VER-01
- **Outcome:** Defined ASan, UBSan, parser fuzzing, malformed-input, and extreme-boundary campaigns.
- **Acceptance planning:** Include monotonic parser progress, no out-of-bounds access, explicit status, and invariant preservation.
- **Recruiting impact:** **Very High** — strong security, compiler, and correctness signal.
- **Journal:** `docs/engineering-journal/03-verification/20-Sanitizer-and-Fuzzing-Strategy.md`
- **Template:** VER

## VER-06 — Establish artifact and replay provenance

- **Priority:** P1
- **Audit findings:** HEL-007, HEL-048
- **Difficulty:** M
- **Dependencies:** BEN-01, INF-03
- **Outcome:** Tie every saved result to source revision, executable hash, compiler, commands, input identity, and output schema.
- **Acceptance planning:** Historical artifacts that cannot be tied to source must be labeled archival/unverified rather than current evidence.
- **Recruiting impact:** **Very High** — materially increases trust in the whole repository.
- **Journal:** `docs/engineering-journal/03-verification/21-Artifact-Provenance.md`
- **Template:** INFRA

---

# Epic 4 — Benchmarking and Profiling

## BEN-01 — Make build and benchmark provenance reproducible

- **Priority:** P1
- **Audit findings:** HEL-005, HEL-043
- **Difficulty:** M
- **Dependencies:** INF-02
- **Outcome:** Exact target flags, compiler version, source revision, binary hash, CPU topology, kernel, run command, and raw result manifest.
- **Acceptance planning:** Ensure the benchmark driver and linked library are built with the intended optimization and architecture settings.
- **Recruiting impact:** **Very High** — converts headline numbers into defensible evidence.
- **Journal:** `docs/engineering-journal/04-benchmarking/22-Benchmark-Provenance.md`
- **Template:** BENCH

## BEN-02 — Produce a complete allocation map of each hot operation

- **Priority:** P2
- **Audit findings:** HEL-006, HEL-035
- **Difficulty:** M
- **Dependencies:** VER-03, COR-07
- **Outcome:** Identify allocations and deallocations for add, cancel, reduce, replace, sweep, pool growth, and rehash.
- **Acceptance planning:** Distinguish order storage, hash nodes, buckets, vectors, aligned allocation, and allocator metadata.
- **Recruiting impact:** **Very High** — directly supports low-latency allocator discussions.
- **Journal:** `docs/engineering-journal/04-benchmarking/23-Hot-Path-Allocation-Map.md`
- **Template:** BENCH

## BEN-03 — Replace the add-only headline with a workload portfolio

- **Priority:** P2
- **Audit findings:** HEL-028
- **Difficulty:** L
- **Dependencies:** VER-02, VER-04, PRO-04
- **Outcome:** Separate benchmarks for first-level add, existing-level add, cancel topology, reduce, replace, partial/full execution, sweep, mixed bounded-live-set, and end-to-end replay.
- **Acceptance planning:** Every workload must publish its state distribution and successful operation mix.
- **Recruiting impact:** **Very High** — creates a professional performance-engineering story rather than one attractive number.
- **Journal:** `docs/engineering-journal/04-benchmarking/24-Workload-Portfolio.md`
- **Template:** BENCH

## BEN-04 — Redesign latency statistics and timer-overhead treatment

- **Priority:** P2
- **Audit findings:** HEL-029
- **Difficulty:** L
- **Dependencies:** BEN-01, BEN-03
- **Outcome:** Clearly separate batch throughput, observed instrumented latency, quantile method, run aggregation, and global maxima.
- **Acceptance planning:** Preserve raw samples; do not label median-of-maxima as worst case; justify rather than assume overhead subtraction.
- **Recruiting impact:** **Very High** — high signal for HRT, Jump, Tower, and performance roles.
- **Journal:** `docs/engineering-journal/04-benchmarking/25-Latency-Statistics.md`
- **Template:** BENCH

## BEN-05 — Verify CPU affinity, TSC capability, and platform gating

- **Priority:** P2
- **Audit findings:** HEL-030, HEL-031, HEL-040
- **Difficulty:** M
- **Dependencies:** BEN-01, INF-01
- **Outcome:** Fail clearly when timing prerequisites are unavailable; record effective affinity and migration evidence.
- **Acceptance planning:** Cover affinity return values, cpuset restrictions, invariant TSC, RDTSCP, AUX, ARM behavior, and benchmark exit status.
- **Recruiting impact:** **Very High** — demonstrates hardware/OS measurement discipline.
- **Journal:** `docs/engineering-journal/04-benchmarking/26-Timing-Platform-Validation.md`
- **Template:** BENCH

## BEN-06 — Separate workload generation from profiled book operations

- **Priority:** P2
- **Audit findings:** HEL-032
- **Difficulty:** M
- **Dependencies:** VER-04, BEN-03
- **Outcome:** Pre-generated or separately attributed operation streams so RNG and administration do not dominate whole-program counters.
- **Acceptance planning:** Report both end-to-end and isolated book profiles with explicit boundaries.
- **Recruiting impact:** **Very High** — repairs attribution of IPC, cache, and branch evidence.
- **Journal:** `docs/engineering-journal/04-benchmarking/27-Profile-Workload-Isolation.md`
- **Template:** BENCH

## BEN-07 — Reinvestigate latency tails with correlated evidence

- **Priority:** P2
- **Audit findings:** HEL-033, HEL-044
- **Difficulty:** L
- **Dependencies:** BEN-02, BEN-04, BEN-05, BEN-06
- **Outcome:** Replace speculation with sample-correlated allocator, page-fault, migration, IRQ, capacity, and code-path evidence.
- **Acceptance planning:** The investigation must allow the conclusion “unresolved”; it must not force an OS-noise narrative.
- **Recruiting impact:** **Very High** — excellent apprenticeship project in scientific debugging.
- **Journal:** `docs/engineering-journal/04-benchmarking/28-Tail-Latency-Investigation.md`
- **Template:** INV

## BEN-08 — Reproduce the custom hash-map experiment

- **Priority:** P3
- **Audit findings:** HEL-045
- **Difficulty:** L
- **Dependencies:** BEN-01 through BEN-04, BEN-02
- **Outcome:** Reconstruct or replace the missing experiment with controlled capacity, load factor, hashing, deletion, growth, and raw results.
- **Acceptance planning:** Conclusion must be scoped to the tested implementation and workload, not “custom maps versus STL.”
- **Recruiting impact:** **Very High** — failed experiments are powerful when reproducible and correctly scoped.
- **Journal:** `docs/engineering-journal/04-benchmarking/29-Hash-Index-Experiment.md`
- **Template:** BENCH

## BEN-09 — Measure bitmap refresh complexity and alternatives

- **Priority:** P3
- **Audit findings:** HEL-027
- **Difficulty:** M
- **Dependencies:** BEN-03
- **Outcome:** Quantify global bitmap scans under sparse/wide price distributions and evaluate local or hierarchical summaries.
- **Acceptance planning:** Include worst-case word scans, realistic distributions, memory cost, and branch/cache counters.
- **Recruiting impact:** **High** — focused algorithm/microarchitecture experiment.
- **Journal:** `docs/engineering-journal/04-benchmarking/30-Bitmap-Refresh-Experiment.md`
- **Template:** BENCH

## BEN-10 — Measure compact versus cache-line-aligned order layouts

- **Priority:** P3
- **Audit findings:** HEL-015, HEL-036
- **Difficulty:** L
- **Dependencies:** BEN-01 through BEN-04
- **Outcome:** Compare working set, cache/TLB behavior, sweep latency, random cancel latency, and memory footprint across layouts.
- **Acceptance planning:** Do not assume either compactness or alignment wins; preserve identical semantics and workload.
- **Recruiting impact:** **Very High** — strong CPU architecture and experimental design signal.
- **Journal:** `docs/engineering-journal/04-benchmarking/31-Order-Layout-Experiment.md`
- **Template:** BENCH

---

# Epic 5 — Documentation

## DOC-01 — Reconcile architecture and evidence documentation

- **Priority:** P2
- **Audit findings:** HEL-014, HEL-048
- **Difficulty:** M
- **Dependencies:** VER-06, BEN-01
- **Outcome:** One current architecture description; historical documents clearly labeled; placeholder flamegraphs and unfinished result fields removed or archived.
- **Acceptance planning:** Every performance table must link to reproducible evidence or be explicitly marked historical/unverified.
- **Recruiting impact:** **Very High** — immediate improvement to trust and reviewer experience.
- **Journal:** `docs/engineering-journal/06-documentation/32-Documentation-Evidence-Reconciliation.md`
- **Template:** DOC

## DOC-02 — Align language-standard and platform claims

- **Priority:** P3
- **Audit findings:** HEL-010
- **Difficulty:** XS
- **Dependencies:** INF-02
- **Outcome:** README, CMake, compiler matrix, and feature usage name the same C++ standard and supported platforms.
- **Recruiting impact:** **Medium** — removes an easy credibility challenge.
- **Journal:** `docs/engineering-journal/06-documentation/33-Language-and-Platform-Contract.md`
- **Template:** DOC

## DOC-03 — Define performance-claim vocabulary

- **Priority:** P2
- **Audit findings:** HEL-023, HEL-028, HEL-029, HEL-033
- **Difficulty:** S
- **Dependencies:** PRO-04, BEN-03, BEN-04, BEN-07
- **Outcome:** Consistent definitions for operation, modeled message, wire message, target event, mean, percentile, maximum, throughput, and end-to-end replay.
- **Recruiting impact:** **Very High** — makes the project sound like professional performance engineering rather than benchmark marketing.
- **Journal:** `docs/engineering-journal/06-documentation/34-Performance-Claim-Vocabulary.md`
- **Template:** DOC

## DOC-04 — Clean repository hygiene and unfinished artifacts

- **Priority:** P3
- **Audit findings:** HEL-016
- **Difficulty:** S
- **Dependencies:** DOC-01
- **Outcome:** Resolve malformed ignore content, duplicate includes/usings, unfinished stubs, dead links, and unexplained disconnected files.
- **Recruiting impact:** **Medium** — improves first-impression professionalism.
- **Journal:** `docs/engineering-journal/06-documentation/35-Repository-Hygiene.md`
- **Template:** DOC

---

# Epic 6 — Infrastructure

## INF-01 — Establish a compiler and verification CI matrix

- **Priority:** P1
- **Audit findings:** HEL-046
- **Difficulty:** M
- **Dependencies:** VER-01; can begin in parallel with VER-02.
- **Outcome:** Debug, Release, ASan, UBSan, multiple compiler, and supported-platform checks with benchmark gating.
- **Acceptance planning:** Functional CI must not treat ARM no-op timing as a successful benchmark.
- **Recruiting impact:** **Very High** — strong professional ownership and infrastructure signal.
- **Journal:** `docs/engineering-journal/05-infrastructure/36-CI-Verification-Matrix.md`
- **Template:** INFRA

## INF-02 — Modernize target-scoped CMake behavior

- **Priority:** P1
- **Audit findings:** HEL-005, HEL-047
- **Difficulty:** M
- **Dependencies:** DOC-02
- **Outcome:** Target-scoped warnings, standard, optimization, architecture options, optional tests, and consistent library/benchmark builds.
- **Acceptance planning:** Preserve user/toolchain flags and expose intended build modes explicitly.
- **Recruiting impact:** **High** — removes benchmark ambiguity and improves composability.
- **Journal:** `docs/engineering-journal/05-infrastructure/37-CMake-Target-Model.md`
- **Template:** INFRA

## INF-03 — Define generated-artifact lifecycle

- **Priority:** P2
- **Audit findings:** HEL-007, HEL-014, HEL-048
- **Difficulty:** M
- **Dependencies:** VER-06, DOC-01
- **Outcome:** Policy for raw results, summarized reports, generated flamegraphs, environment manifests, retention, and archival status.
- **Acceptance planning:** Generated reports cannot silently overwrite or masquerade as current canonical evidence.
- **Recruiting impact:** **High** — shows reproducible research and repository stewardship.
- **Journal:** `docs/engineering-journal/05-infrastructure/38-Generated-Artifact-Lifecycle.md`
- **Template:** INFRA

---

# Epic 7 — Future Work

Future work is deliberately P4 unless it directly supports an earlier verification experiment. These items must not displace correctness or evidence work.

## FUT-01 — Reassess the generic `ObjectPool<T>` contract

- **Priority:** P3
- **Audit findings:** HEL-013
- **Difficulty:** M
- **Dependencies:** COR-06, COR-07
- **Outcome:** Either constrain the pool to suitable trivial types or define destruction for still-live non-trivial objects.
- **Recruiting impact:** **High** — useful C++ object-lifetime case study.
- **Journal:** `docs/engineering-journal/07-experiments/39-Object-Pool-Lifetime-Contract.md`
- **Template:** ADR

## FUT-02 — Design multi-symbol single-writer sharding

- **Priority:** P4
- **Audit findings:** HEL-024, HEL-036, HEL-042
- **Difficulty:** XL
- **Dependencies:** COR-01, COR-08, PRO-03, PRO-05, VER-02
- **Outcome:** Ownership and routing model that avoids sharing one mutable book across threads and avoids dense full-range ladders per inactive instrument.
- **Recruiting impact:** **High**, but only after correctness — valuable architecture discussion, weak if the single-symbol core remains unverified.
- **Journal:** `docs/engineering-journal/08-future/40-Multi-Symbol-Sharding-RFC.md`
- **Template:** RFC

## FUT-03 — Evaluate alternative order-ID indexes

- **Priority:** P4
- **Audit findings:** HEL-006, HEL-035, HEL-045
- **Difficulty:** L
- **Dependencies:** BEN-02, BEN-03, BEN-08
- **Outcome:** Evidence-based comparison of node hash, flat hash, pooled nodes, and any reference-number-aware indexing.
- **Recruiting impact:** **High** — only if run as a controlled experiment.
- **Journal:** `docs/engineering-journal/07-experiments/41-Order-ID-Index-Alternatives.md`
- **Template:** BENCH

## FUT-04 — Evaluate segmented or compressed price ladders

- **Priority:** P4
- **Audit findings:** HEL-001, HEL-017, HEL-027, HEL-036
- **Difficulty:** XL
- **Dependencies:** COR-01, BEN-09, FUT-02
- **Outcome:** Preserve direct-access advantages without reserving the entire protocol price domain per symbol.
- **Recruiting impact:** **Very High** — rich data-structure and workload-density project after the price model is correct.
- **Journal:** `docs/engineering-journal/07-experiments/42-Price-Ladder-Alternatives.md`
- **Template:** BENCH

## FUT-05 — Investigate prefetch and NUMA behavior

- **Priority:** P4
- **Audit findings:** HEL-015, HEL-036
- **Difficulty:** L
- **Dependencies:** BEN-06, BEN-10, reproducible Linux hardware
- **Outcome:** Determine whether long sweeps, routing, pool placement, or memory binding benefit from explicit intervention.
- **Recruiting impact:** **High** for performance roles; low value without counter evidence.
- **Journal:** `docs/engineering-journal/07-experiments/43-Prefetch-and-NUMA-Study.md`
- **Template:** BENCH

## FUT-06 — Design real-time ingestion boundaries

- **Priority:** P4
- **Audit findings:** HEL-037, HEL-042
- **Difficulty:** XL
- **Dependencies:** PRO-02, FUT-02
- **Outcome:** RFC for sequencing, gap recovery, redundant feeds, backpressure, and handoff into single-writer book shards.
- **Recruiting impact:** **Medium to High** — valuable only as a grounded architecture extension, not as a kernel-bypass feature checklist.
- **Journal:** `docs/engineering-journal/08-future/44-Real-Time-Ingestion-RFC.md`
- **Template:** RFC

## FUT-07 — Decide whether Helios will ever include matching-engine semantics

- **Priority:** P4
- **Audit findings:** HEL-004, HEL-039, HEL-042
- **Difficulty:** XL
- **Dependencies:** COR-08 and completion of the reconstruction-book verification foundation
- **Outcome:** Explicit go/no-go decision with separate APIs, invariants, and benchmarks if matching is pursued.
- **Recruiting impact:** **Medium** — a clear decision is more valuable than an ambiguous partial implementation.
- **Journal:** `docs/engineering-journal/08-future/45-Matching-Engine-Scope-RFC.md`
- **Template:** RFC

---

# Audit Finding Coverage

Every audit finding is assigned to at least one backlog item.

| Findings | Primary backlog item(s) |
|---|---|
| HEL-001, HEL-017 | COR-01; later FUT-04 |
| HEL-002 | COR-02 |
| HEL-003 | COR-03 |
| HEL-004 | COR-04; COR-08; FUT-07 |
| HEL-005 | BEN-01, INF-02 |
| HEL-006 | BEN-02; later FUT-03 |
| HEL-007 | VER-06, INF-03 |
| HEL-008 | VER-03 |
| HEL-009 | VER-02 |
| HEL-010 | DOC-02 |
| HEL-011 | VER-01, VER-02, VER-05 |
| HEL-012 | PRO-01 |
| HEL-013 | FUT-01 |
| HEL-014 | DOC-01, INF-03 |
| HEL-015 | BEN-10; later FUT-05 |
| HEL-016 | DOC-04 |
| HEL-018, HEL-019, HEL-020 | COR-05 |
| HEL-021 | COR-06 |
| HEL-022 | PRO-01, VER-05 |
| HEL-023 | PRO-04, DOC-03 |
| HEL-024 | PRO-03; later FUT-02 |
| HEL-025 | PRO-01 |
| HEL-026 | PRO-06 |
| HEL-027 | BEN-09; later FUT-04 |
| HEL-028 | BEN-03, DOC-03 |
| HEL-029 | BEN-04, DOC-03 |
| HEL-030, HEL-031, HEL-040 | BEN-05 |
| HEL-032 | BEN-06 |
| HEL-033, HEL-044 | BEN-07, DOC-03 |
| HEL-034 | VER-04 |
| HEL-035 | COR-07, BEN-02; later FUT-03 |
| HEL-036 | BEN-10; later FUT-02, FUT-04, FUT-05 |
| HEL-037 | PRO-02; later FUT-06 |
| HEL-038 | PRO-05 |
| HEL-039, HEL-041 | COR-09 |
| HEL-042 | COR-08; later FUT-02, FUT-06, FUT-07 |
| HEL-043 | BEN-01 |
| HEL-045 | BEN-08; later FUT-03 |
| HEL-046 | INF-01, VER-05 |
| HEL-047 | INF-02 |
| HEL-048 | VER-06, DOC-01, INF-03 |

---

# Recommended Engineering Journal Structure

The journal should remain separate from polished architecture documentation. It is the record of hypotheses, decisions, failed attempts, measurements, limitations, and changes in understanding.

```text
docs/
└── engineering-journal/
    ├── README.md
    ├── 00-methodology/
    │   ├── Evidence-Standards.md
    │   ├── Benchmark-Environment-Checklist.md
    │   └── Definition-of-Done.md
    ├── 01-correctness/
    ├── 02-protocol/
    ├── 03-verification/
    ├── 04-benchmarking/
    ├── 05-infrastructure/
    ├── 06-documentation/
    ├── 07-experiments/
    ├── 08-future/
    ├── templates/
    │   ├── ADR.md
    │   ├── Investigation.md
    │   ├── Verification.md
    │   ├── Benchmark.md
    │   ├── Infrastructure.md
    │   ├── Documentation.md
    │   └── RFC.md
    └── index.md
```

## Journal rules

1. One journal entry per backlog item; use the filename assigned above.
2. Create the entry before implementation begins.
3. Record hypotheses before collecting results.
4. Preserve failed experiments and counterevidence.
5. Link raw artifacts rather than copying only selected summary numbers.
6. Separate repository facts, external facts, engineering inference, and unresolved questions.
7. Never rewrite history to make an experiment look more linear than it was; append dated amendments.
8. End every completed entry with what changed in the engineer’s mental model.

---

# Reusable Journal Templates

The assigned journal files should contain only the relevant reusable template when initially created. They should not be pre-populated with audit conclusions or expected results.

## ADR template

```markdown
# <Decision Title>

- Backlog ID:
- Status: Proposed | Accepted | Superseded | Rejected
- Date opened:
- Date decided:
- Owners:
- Related findings:

## Context

<What system boundary or problem requires a decision?>

## Decision question

<State one precise question.>

## Required invariants

- <Invariant>

## Constraints

- <Constraint>

## Options considered

### Option A — <Name>

- Mechanism:
- Advantages:
- Disadvantages:
- Failure modes:
- Measurement required:

### Option B — <Name>

- Mechanism:
- Advantages:
- Disadvantages:
- Failure modes:
- Measurement required:

## Decision

<Leave blank until evidence supports a decision.>

## Consequences

- Positive:
- Negative:
- Reversibility:

## Validation plan

- <How the decision will be falsified or verified>

## Evidence and artifacts

- <Links to tests, raw data, profiles, specifications, or commits>

## Unresolved questions

- <Question>

## Interview reflection

- What tradeoff should I be able to explain?
- What evidence would a skeptical interviewer request?

## Amendments

- <Date — new evidence or superseding decision>
```

## Investigation template

```markdown
# <Investigation Title>

- Backlog ID:
- Status: Open | Confirmed | Rejected | Inconclusive
- Date opened:
- Owners:
- Related findings:

## Observed symptom

<Describe the observation without asserting a cause.>

## Competing hypotheses

| Hypothesis | Predicted evidence | Falsifying evidence |
|---|---|---|
| <Hypothesis> | <Prediction> | <Falsifier> |

## Reproduction conditions

- Source revision:
- Build configuration:
- Hardware/OS:
- Input/workload:
- Command:

## Instrumentation plan

- <Signal, counter, trace, or invariant>

## Observations

| Observation | Evidence class | Artifact |
|---|---|---|
| <Observation> | Repository | External | Measured | Inference | <Link> |

## Root-cause assessment

- Conclusion:
- Confidence:
- Alternative explanations not eliminated:

## Decision or follow-up

- <Next action>

## Learning reflection

- What assumption changed?
- Which diagnostic was most informative?
- What would I do first next time?

## Amendments

- <Date — additional evidence>
```

## Verification template

```markdown
# <Verification Title>

- Backlog ID:
- Status: Planned | Active | Passing | Failing | Superseded
- Date opened:
- Owners:
- Related findings:

## Contract under verification

<State observable behavior, not implementation details.>

## Oracle

- Reference source/model:
- Why it is independent:
- Known limitations:

## Invariants

- <Invariant>

## Test dimensions

| Dimension | Partitions and boundaries |
|---|---|
| <Input dimension> | <Cases> |

## Positive scenarios

- <Scenario>

## Negative and malformed scenarios

- <Scenario>

## Generated/property scenarios

- Generator model:
- Shrinking/reproduction strategy:
- Seed recording policy:

## Acceptance criteria

- <Criterion>

## Results

- Source revision:
- Commands:
- Artifacts:
- Outcome:

## Coverage limitations

- <What remains unverified?>

## Learning reflection

- Which invariant caught the most important defect?
- Which test looked useful but was not discriminating?
```

## Benchmark template

```markdown
# <Experiment Title>

- Backlog ID:
- Status: Planned | Running | Complete | Inconclusive | Superseded
- Date opened:
- Owners:
- Related findings:

## Question

<One measurable question.>

## Hypothesis recorded before measurement

- Expected outcome:
- Mechanism:
- Evidence that would reject it:

## Compared variants

| Variant | Description | Expected tradeoff |
|---|---|---|
| <Variant> | <Description> | <Tradeoff> |

## Controlled variables

- <Variable held constant>

## Environment manifest

- Source revision:
- Dirty-worktree status:
- Compiler and version:
- Full compile/link commands:
- Binary hash:
- CPU and topology:
- Memory/NUMA:
- OS/kernel:
- Isolation/affinity:
- Frequency/power policy:

## Workload definition

- Input distribution:
- Live-state distribution:
- Warm-up:
- Sample count:
- Repetition strategy:
- Timed boundary:
- Operations excluded from timing:

## Metrics and statistical method

- Primary metric:
- Secondary metrics:
- Quantile definition:
- Outlier policy:
- Timer-overhead policy:

## Raw artifacts

- Samples:
- Counters:
- Profiles/flamegraphs:
- Logs:

## Results

| Variant | Metric | Result | Uncertainty/stability |
|---|---|---:|---|
| <Variant> | <Metric> | <Value> | <Assessment> |

## Counterevidence and confounders

- <Confounder>

## Conclusion

- Supported claim:
- Unsupported claim:
- Decision:

## Learning reflection

- What surprised me?
- Which variable mattered most?
- What experiment should follow?
```

## Infrastructure template

```markdown
# <Infrastructure Change Title>

- Backlog ID:
- Status: Proposed | Active | Complete | Superseded
- Date opened:
- Owners:
- Related findings:

## Problem

<What is unreliable, ambiguous, manual, or unreproducible?>

## Users and workflows

- <User/workflow>

## Current behavior

- <Observed behavior>

## Proposed contract

- <Stable behavior the infrastructure should provide>

## Compatibility constraints

- Platforms:
- Compilers:
- Build modes:
- Existing workflows:

## Failure modes

| Failure | Detection | Required response |
|---|---|---|
| <Failure> | <Detection> | <Response> |

## Validation matrix

| Environment | Configuration | Expected result |
|---|---|---|
| <Environment> | <Configuration> | <Result> |

## Rollout and rollback

- Rollout:
- Rollback:

## Evidence and artifacts

- <Link>

## Learning reflection

- Which hidden dependency was discovered?
- How did the change improve reproducibility?
```

## Documentation template

```markdown
# <Documentation Work Title>

- Backlog ID:
- Status: Planned | Active | Complete | Superseded
- Date opened:
- Owners:
- Related findings:

## Audience

- <Reader and expected background>

## Claim inventory

| Claim | Evidence class | Supporting artifact | Current confidence |
|---|---|---|---|
| <Claim> | Repository | External | Measured | Inference | <Link> | <Confidence> |

## Contradictions or stale material

- <Contradiction>

## Required terminology

- <Term and exact definition>

## Proposed structure

- <Section>

## Evidence requirements

- <Requirement>

## Review checklist

- [ ] Current source matches architecture description
- [ ] Performance claims link to reproducible artifacts
- [ ] Historical results are labeled
- [ ] Unsupported claims are removed or qualified
- [ ] Links and commands are valid

## Learning reflection

- Which claim was hardest to support?
- What would a skeptical reviewer misunderstand?
```

## RFC template

```markdown
# <RFC Title>

- Backlog ID:
- Status: Draft | Under Review | Accepted | Rejected | Deferred
- Date opened:
- Owners:
- Related findings:

## Problem statement

<What future capability or scaling limit is being addressed?>

## Non-goals

- <Explicit non-goal>

## Preconditions

- <Earlier correctness/evidence milestone that must be complete>

## Requirements

### Functional

- <Requirement>

### Performance

- <Requirement and measurement method>

### Reliability

- <Requirement>

## Proposed architecture

<Describe components, ownership, interfaces, and data flow.>

## Alternatives

| Alternative | Advantages | Disadvantages | Rejection criteria |
|---|---|---|---|
| <Alternative> | <Advantages> | <Disadvantages> | <Criteria> |

## State and ownership model

- <Owner and lifetime>

## Failure and recovery model

- <Failure mode and recovery>

## Migration strategy

- <Incremental path>

## Verification and benchmark plan

- <How correctness and performance will be established>

## Risks and unknowns

- <Risk>

## Decision

<Leave blank until review is complete.>

## Learning reflection

- What new systems concept does this RFC require mastering?
- What evidence must exist before implementation is justified?
```

---

# Learning-First Implementation Order

This order intentionally performs model-building and verification before optimization. It is slower than patching visible defects directly, but it maximizes durable engineering learning.

## Stage 0 — Preserve and classify existing evidence

1. VER-06 — Artifact and replay provenance
2. BEN-01 — Benchmark/build provenance
3. DOC-01 — Documentation evidence reconciliation
4. INF-03 — Generated-artifact lifecycle

**Learning objective:** Distinguish what is known, measured, inferred, and irreproducible before changing the system that produced the evidence.

## Stage 1 — Define the semantic model

5. COR-01 — Price domain
6. COR-02 — ID uniqueness
7. COR-03 — Zero quantity
8. COR-04 — Modify/priority semantics
9. COR-05 — Numeric boundaries
10. COR-08 — Reconstruction versus matching

**Learning objective:** Practice defining invariants and domain contracts independently of current implementation convenience.

## Stage 2 — Build independent verification before fixing behavior

11. VER-01 — ITCH golden fixtures
12. VER-02 — Reference book and differential verification
13. INF-01 — CI verification matrix
14. VER-05 — Sanitizer/fuzzing strategy
15. VER-04 — Workload lifecycle accounting

**Learning objective:** Build oracles capable of proving that later changes are correct and of discovering defects not listed in the audit.

## Stage 3 — Correct state transitions and failure behavior

16. COR-02 through COR-05 implementation work, in dependency order
17. COR-06 — Exception safety
18. COR-07 — Capacity/growth policy
19. FUT-01 — Pool lifetime contract
20. COR-09 — Public API clarity

**Learning objective:** Develop strong ownership, rollback, and state-machine reasoning in C++.

## Stage 4 — Complete the protocol boundary

21. PRO-01 — Structured parser outcomes
22. PRO-02 — Session/lifecycle integrity
23. PRO-03 — Stock-locate routing
24. PRO-05 — Feed metadata lifecycle
25. PRO-06 — Mapping/prefault contract
26. PRO-04 — Replay metric semantics

**Learning objective:** Move from byte decoding to professional protocol-processing semantics and operational diagnostics.

## Stage 5 — Rebuild the measurement foundation

27. INF-02 — Target-scoped CMake
28. VER-03 — Allocation verification
29. BEN-02 — Hot-path allocation map
30. BEN-03 — Workload portfolio
31. BEN-04 — Latency statistics
32. BEN-05 — Timing platform validation
33. BEN-06 — Profile workload isolation
34. BEN-07 — Tail investigation

**Learning objective:** Learn to design experiments that can reject attractive hypotheses and support only narrowly stated conclusions.

## Stage 6 — Run controlled optimization experiments

35. BEN-08 — Hash index
36. BEN-09 — Bitmap refresh
37. BEN-10 — Order layout
38. FUT-03 — Order-ID index alternatives
39. FUT-05 — Prefetch/NUMA study
40. FUT-04 — Price-ladder alternatives

**Learning objective:** Connect algorithms, allocators, cache/TLB behavior, CPU counters, and workload shape without optimizing by aesthetic preference.

## Stage 7 — Publish a truthful recruiting artifact

41. DOC-02 — Language/platform contract
42. DOC-03 — Performance vocabulary
43. DOC-04 — Repository hygiene
44. Final evidence-backed README and architecture refresh under DOC-01

**Learning objective:** Communicate tradeoffs and limitations at the same standard used to build and measure the system.

## Stage 8 — Consider architectural expansion

45. FUT-02 — Multi-symbol sharding
46. FUT-06 — Real-time ingestion
47. FUT-07 — Matching-engine scope decision

**Learning objective:** Design larger systems only after the single-threaded core has a trustworthy specification, oracle, and performance baseline.

## Explicitly deferred feature work

Kernel bypass, lock-free shared mutation, dashboards, FPGA parsing, and unrelated feature expansion should remain outside the active backlog until Stages 0–5 are complete. Adding them earlier would increase surface area while weakening the repository’s central recruiting story: measured, evidence-driven systems engineering.
