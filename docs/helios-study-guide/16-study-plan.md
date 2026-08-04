# Study Plan

The curriculum treats Helios as a laboratory. The recurring loop is: predict state, trace code, state invariants, test the claim mentally, explain aloud, then compare with evidence. Do not begin by memorizing benchmark numbers.

## Practice protocol

For every study session:

1. Spend five minutes recalling yesterday's model without notes.
2. Read one shared chapter and its linked per-file guides.
3. Trace at least one operation on paper, including every representation touched.
4. Give a timed oral explanation.
5. Write one falsifiable question or experiment, without implementing it.
6. Schedule review after 1, 3, 7, 14, and 30 days.

Use these confidence levels:

- **L1 Recognition:** you understand the explanation when reading it.
- **L2 Recall:** you can state the mechanism without notes.
- **L3 Derivation:** you can reconstruct it from invariants on a whiteboard.
- **L4 Defense:** you can answer hostile alternatives and limitations precisely.
- **L5 Extension:** you can predict how a change affects correctness, ownership, CPU behavior, tests, and measurement.

## Seven-day crash course

### Day 1 — Market model and project scope

- Read [01](01-project-from-first-principles.md), [00](00-learning-map.md), and the `types`, `order`, and `price_level` file guides.
- Manually build a two-sided book from six orders; compute best prices, spread, aggregates, and FIFO.
- Explain aloud: “displayed reconstruction versus matching engine” in 30 seconds and two minutes.
- Without notes: define bid, ask, price level, Price(4), order reference, partial execution.
- Checkpoint: draw one order's lifecycle from Add through partial Execute to Delete.

### Day 2 — Core book representations

- Read [04](04-data-structures-and-invariants.md) and the `orderbook.hpp/.cpp` guides.
- Trace Add and Cancel, writing the pool, map, queue, aggregate, bitmap, best cache, and global count after each statement block.
- Whiteboard: prove head/middle/tail unlink and empty-level transitions.
- Without notes: state invariants I1–I19 from [04] in your own grouping.
- Checkpoint: explain exactly how duplicate ID insertion violates three invariants.

### Day 3 — Ownership and C++ lifetime

- Read [05](05-memory-ownership-and-lifetimes.md) and `object_pool.hpp` guide.
- Draw the ownership graph and separately draw every non-owning pointer.
- Trace acquire/release using a `Slot` first as free-list state, then as live `Order` storage.
- Oral question: “Why does placement new not allocate?”
- Checkpoint: explain generic pool teardown debt and Add exception rollback.

### Day 4 — Protocol and replay

- Read [07](07-itch-protocol-and-replay-semantics.md), then parser/replay guides.
- On paper decode one A, E, X, D, and U fixture byte by byte.
- Demonstrate the 10001/10099 Price(4) collision.
- Draw framing → decoding → filtering → lifecycle → accepted mutation counters.
- Checkpoint: state exact supported lengths and explain why minimum-length validation is incomplete.

### Day 5 — CPU and performance

- Read [06](06-cpu-cache-and-performance-model.md) and [11](11-engineering-decisions-and-tradeoffs.md).
- Draw a likely cache-miss chain for random cancellation.
- Compare direct ladder, tree, and segmented ladder across density/range/memory.
- Oral question: “Does `alignas(64)` improve performance?” Give a hypothesis, not a verdict.
- Checkpoint: distinguish instruction latency, throughput, MLP, TLB miss, page fault, and false sharing.

### Day 6 — Measurement, tests, and build

- Read [08](08-benchmarking-and-measurement.md), [09](09-testing-and-correctness.md), and [10](10-build-system-and-tooling.md).
- For every benchmark write its timed boundary and denominator.
- Design a reference oracle and a transaction fault-injection table.
- Trace flags from CMake target to each translation unit.
- Checkpoint: answer “What does 26.6 ns prove?” and “Why is the allocation check invalid?”

### Day 7 — Audit and oral defense

- Read [12](12-technical-debt-and-limitations.md), [13](13-production-redesign.md), and [14](14-interview-defense-guide.md).
- Sort HEL-001–048 by violated contract rather than ID.
- Perform the six mock whiteboard exercises in [14].
- Give a ten-minute project defense, recording yourself.
- Final checkpoint: name what you would correct first, what you would measure next, and what you would explicitly not claim.

## Thirty-day deep study plan

Each week includes five focused days, one synthesis day, and one recovery/review day.

### Week 1 — State-machine correctness

| Day | Reading | Trace/exercise | Oral result |
|---:|---|---|---|
| 1 | 01, types/order guides | Tiny book vocabulary | 2-minute scope |
| 2 | price-level guide | Append/unlink all cases | FIFO proof |
| 3 | order-book declarations | Draw full state | ownership versus indexing |
| 4 | Add implementation | Failure point table | transactional Add |
| 5 | cancel/reduce/modify | Zero/full/invalid cases | semantics matrix |
| 6 | market sweep | Multi-level fill ledger | why not matching |
| 7 | Review | Blank-page architecture | L3 checkpoint |

Questions to answer: Which representation is canonical for identity? For price occupancy? Can aggregate quantity be canonical? What must be rolled back if map insertion throws? Which public inputs need domain rules?

### Week 2 — Protocol correctness

| Day | Reading | Trace/exercise | Oral result |
|---:|---|---|---|
| 8 | BinaryFILE + parser | Truncated prefix/payload | framing contract |
| 9 | A/F | Offset table from memory | Add semantics |
| 10 | E/C/X/D | Quantity lifecycle | anomaly policy |
| 11 | U | old/new identity timeline | replace atomicity |
| 12 | replay adapter | Symbol/non-symbol paths | counter taxonomy |
| 13 | HEL-001/017/022/024 | Counterexamples | protocol defense |
| 14 | Review | Design golden fixtures | L3 checkpoint |

Questions: Which messages establish symbol identity? Why can later messages omit the symbol? What is the exact Price(4) scale? What is lost if metadata is discarded? How should malformed differ from unsupported?

### Week 3 — Machine and measurement model

| Day | Reading | Trace/exercise | Oral result |
|---:|---|---|---|
| 15 | CPU pipelines/cache | Random cancel load chain | bottleneck hypothesis |
| 16 | TLB/pages/mmap | Cold/warm replay timeline | residency model |
| 17 | pool/hash allocation | Capacity thresholds | hot/slow paths |
| 18 | timer guide | Start/end ordering | TSC caveats |
| 19 | benchmarks | Boundary table | claim calibration |
| 20 | artifacts/profiling | Provenance manifest | evidence defense |
| 21 | Review | Design A/B experiments | L3 checkpoint |

Questions: Why can compacting `Order` help? When could it hurt? What is the end-to-end cancellation dependency chain? Which effects are measured, inferred, or unknown? Why is an observed maximum not a bound?

### Week 4 — Verification and architecture judgment

| Day | Reading | Trace/exercise | Oral result |
|---:|---|---|---|
| 22 | existing tests | Kill/mutation matrix | false confidence |
| 23 | oracle design | Generated trace of 20 ops | differential method |
| 24 | fuzz/sanitizers | Layered test pyramid | verification plan |
| 25 | build graph | Flag propagation | reproducible build |
| 26 | decisions | Ladder alternatives table | tradeoff defense |
| 27 | production redesign | Multi-symbol whiteboard | scale model |
| 28 | all findings | Explain 12 random IDs | limitation fluency |
| 29 | mock interview | 45-minute recording | challenge responses |
| 30 | synthesis | Closed-book system map | L4 checkpoint |

End-of-month standard: explain each major subsystem at beginner, peer, professor, and senior-HFT levels; reproduce the architecture/invariant diagrams from memory; diagnose a novel mutation sequence; refuse unjustified performance claims precisely.

## Ninety-day systems apprenticeship

This plan prioritizes engineering learning, not feature count. “Lab” means design or analysis unless a later, separate task authorizes code changes.

### Days 1–30 — Foundations and faithful reconstruction

Use the deep plan above. Produce private notes for every source-order walkthrough and a one-page correction sheet after each oral recording. Focus on units, state transitions, ownership, and protocol boundaries.

Milestone: you can derive current behavior from source and name where documentation diverges from it.

### Days 31–45 — Specification and verification apprenticeship

- Days 31–33: derive an executable-style contract for Add, Cancel, Reduce, Modify, Delete, Replace, and synthetic Execute.
- Days 34–36: specify the slow reference model and canonical state comparison.
- Days 37–39: enumerate generators, invalid-operation classes, and shrinkable failure traces.
- Days 40–42: design exact ITCH field fixtures, malformed matrices, and callback lifetime tests.
- Days 43–44: design fault injection for pool growth, map insertion, and allocation failure.
- Day 45: oral defense—“How I would know the book is correct.”

Milestone: given an operation sequence, you can predict acceptance, exact state, invariant checks, and anomaly category.

### Days 46–60 — Performance-engineering apprenticeship

- Days 46–48: create a hypothesis table for layout, ladder, bitmap, hash, pool, and parser choices.
- Days 49–51: specify microbenchmarks isolating each mechanism and slow path.
- Days 52–54: specify trace benchmarks and workload distributions; separate state-size phases.
- Days 55–56: design a timer-validation and OS-control checklist.
- Days 57–58: design the experiment manifest/raw-data schema.
- Day 59: interpret fictional counter/latency results and list alternative explanations.
- Day 60: oral defense—“What the measurements do and do not prove.”

Milestone: every performance statement you make includes operation, boundary, workload, state, build, platform, statistic, and uncertainty.

### Days 61–75 — Architecture and production reasoning

- Days 61–63: sharding and single-writer ownership.
- Days 64–66: sequencing, gaps, redundancy, and recovery.
- Days 67–69: capacity, backpressure, and overload/failure policy.
- Days 70–71: snapshots and deterministic replay validation.
- Days 72–73: observability without contaminating the hot path.
- Days 74–75: deployment/NUMA/environment isolation.

Milestone: draw [13's](13-production-redesign.md) architecture from memory and label where order, buffer, and metadata ownership changes.

### Days 76–90 — Interview synthesis and judgment

- Days 76–78: 30-second, two-minute, and ten-minute explanations for each subsystem.
- Days 79–81: defend three alternatives for every major design decision.
- Days 82–84: randomly select and teach four HEL findings per day.
- Days 85–86: solve mock whiteboards with a 20-minute limit.
- Days 87–88: conduct two full hostile interviews; transcribe weak moments.
- Day 89: final closed-book architecture, ownership, invariant, and benchmark maps.
- Day 90: teach the entire project to a classmate and answer questions only from derived principles.

Milestone: admit limitations without underselling the sound ideas, and propose verification before optimization.

## Spaced-review deck

Create cards in these forms:

- **Definition:** “What is Price(4), exactly?”
- **Mechanism:** “How does removing the last best ask change five structures?”
- **Counterexample:** “Give two distinct legal prices current replay merges.”
- **Boundary:** “When is pool acquisition allocation-free?”
- **Evidence:** “Which current test independently verifies FIFO?”
- **Alternative:** “When does a tree beat the direct ladder?”
- **Failure:** “What if map insertion throws after pool acquisition?”
- **Scale:** “How does one-symbol state become single-writer shards?”

Review failed cards the next day; successful cards at 3, 7, 14, and 30 days. A card is not mastered until you can give both the precise answer and one concrete Helios example.

## Closed-book checkpoints

### Checkpoint A — 15 minutes

Draw all core objects and ownership arrows. Trace Add and Cancel. State seven primary invariants. Explain HEL-001 and HEL-002.

### Checkpoint B — 30 minutes

Decode a supported message from an offset table, trace it through callback/filter/book, list every possible rejection category, and explain object lifetime at each boundary.

### Checkpoint C — 45 minutes

Defend each data-structure choice against two alternatives, identify the workload assumptions, and propose a discriminating experiment.

### Checkpoint D — 60 minutes

Give the project defense, accept a hostile change (“support 8,000 symbols and live redundant feeds”), redesign ownership and sequencing, then explain which existing code remains reusable.

## Graduation questions

Answer all without notes:

1. What exactly does Helios implement, and what does it not implement?
2. What are the canonical units at every boundary?
3. What is the complete mutation transaction for each lifecycle event?
4. Which pointers own, which observe, and when do they expire?
5. What are the strongest full-book invariants?
6. Where can allocation occur despite the pool?
7. What makes direct ladders win or lose?
8. What causes the likely critical load chain on random cancellation?
9. How do framing validity, decoder support, symbol relevance, and mutation acceptance differ?
10. How would a reference model expose current tests' blind spots?
11. What exact experiment would validate each optimization?
12. Which historical claims are unverified, and why?
13. Why is replay deterministic but not necessarily complete?
14. How would production sequencing and recovery surround the current core?
15. What are your first three learning-driven improvements, and what would each teach?

Passing means your answers are scoped, mechanistic, and falsifiable—not that they portray the repository as finished.
