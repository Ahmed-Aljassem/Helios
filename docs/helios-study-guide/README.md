# Helios Private Study Guide

This directory is a private textbook for understanding, defending, debugging, and extending Helios. It is deliberately different from the public project README: it teaches the code, preserves uncertainty, and names defects rather than marketing the project.

No claim in this guide should be treated as a new measurement. Important claims use these labels:

- **Repository evidence** — directly visible in tracked source, configuration, tests, or saved artifacts.
- **Externally verified fact** — checked against an official primary source.
- **Engineering inference** — a reasoned prediction about behavior that still needs measurement.
- **Historical/unverified claim** — recorded by the project but not reproducibly tied to the current source and environment.

## How to use the guide

Read the numbered chapters in order on a first pass. On later passes, use the per-file guides while keeping the shared concept chapters open.

| Goal | Start here | Then use |
|---|---|---|
| Understand markets and order books | [01 — First principles](01-project-from-first-principles.md) | [03 — Execution flows](03-end-to-end-execution-flows.md) |
| Understand the program | [02 — Architecture](02-complete-architecture.md) | [`files/`](files/) |
| Debug state corruption | [04 — Invariants](04-data-structures-and-invariants.md) | [05 — Ownership](05-memory-ownership-and-lifetimes.md), [12 — Debt](12-technical-debt-and-limitations.md) |
| Explain performance | [06 — CPU model](06-cpu-cache-and-performance-model.md) | [08 — Measurement](08-benchmarking-and-measurement.md) |
| Defend ITCH replay | [07 — ITCH](07-itch-protocol-and-replay-semantics.md) | Parser and replay file guides |
| Prepare for interviews | [14 — Interview defense](14-interview-defense-guide.md) | [15 — Glossary](15-glossary.md) |
| Plan a production evolution | [13 — Production redesign](13-production-redesign.md) | [11 — Tradeoffs](11-engineering-decisions-and-tradeoffs.md) |

## Textbook volumes

1. [Learning map](00-learning-map.md)
2. [Project from first principles](01-project-from-first-principles.md)
3. [Complete architecture](02-complete-architecture.md)
4. [End-to-end execution flows](03-end-to-end-execution-flows.md)
5. [Data structures and invariants](04-data-structures-and-invariants.md)
6. [Memory ownership and lifetimes](05-memory-ownership-and-lifetimes.md)
7. [CPU, cache, and performance model](06-cpu-cache-and-performance-model.md)
8. [ITCH protocol and replay semantics](07-itch-protocol-and-replay-semantics.md)
9. [Benchmarking and measurement](08-benchmarking-and-measurement.md)
10. [Testing and correctness](09-testing-and-correctness.md)
11. [Build system and tooling](10-build-system-and-tooling.md)
12. [Engineering decisions and tradeoffs](11-engineering-decisions-and-tradeoffs.md)
13. [Technical debt and limitations](12-technical-debt-and-limitations.md)
14. [Production redesign](13-production-redesign.md)
15. [Interview defense guide](14-interview-defense-guide.md)
16. [Glossary](15-glossary.md)
17. [Study plan](16-study-plan.md)

## Project maps

- [Creation plan and completion checklist](COMPLETION_CHECKLIST.md)
- [Tracked-file coverage matrix](TRACKED_FILE_COVERAGE.md)
- [Per-file guides](files/README.md)

## The one-sentence model

Helios reads a historical length-prefixed Nasdaq byte stream, decodes selected order-lifecycle messages, filters additions for one symbol, and applies those events to a single-threaded in-memory displayed-order book built from a hash ID index, dense price ladders, occupancy bitmaps, intrusive FIFO queues, and pooled order storage.

## The most important intellectual distinction

Helios is currently a **displayed market-data reconstruction book** with an additional synthetic liquidity-consumption method. It is not an exchange order-entry matching engine. The replay observes decisions Nasdaq already made; it does not decide whether incoming customer orders match.

## Known central defect

The current replay converts ITCH `Price(4)` values to cents by integer division. This loses legal price precision and can merge distinct levels. The guide explains the defect because understanding it is more educational than hiding it. See [07 — Price representation](07-itch-protocol-and-replay-semantics.md#the-price-precision-defect) and `HEL-001` in [12 — Technical debt](12-technical-debt-and-limitations.md).

## Source navigation convention

Per-file chapters cite function names and approximate source ranges rather than copying large code blocks. Line numbers refer to the audited repository state and may move after later implementation work.

## Recommended oral practice

After each chapter, close the document and answer three questions:

1. What invariant does this subsystem protect?
2. What memory does it own, borrow, allocate, or free?
3. Which performance claim is established, inferred, or still unverified?

