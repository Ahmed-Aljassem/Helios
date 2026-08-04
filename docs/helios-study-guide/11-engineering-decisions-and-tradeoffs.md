# 11 — Engineering Decisions and Tradeoffs

Use this chapter to practice judgment. A senior answer names workload assumptions, evidence, missing evidence, failure modes, and reversibility.

## 1. Direct ladder versus `std::map`

- **Problem:** locate a level and maintain price order.
- **Choice:** dense arrays indexed by price offset.
- **Likely rationale:** eliminate O(log P) tree walks, nodes, and scattered level storage.
- **Measured evidence:** historical artifact reports a large add improvement after array conversion; exact provenance is incomplete.
- **Upside:** computed address, contiguous levels, no per-level allocation.
- **Downside:** O(price range) memory/startup and hard range/scale coupling.
- **Hidden assumption:** the instrument tick domain is narrow and dense.
- **Alternatives:** tree/B-tree, segmented ladder, radix structure, active-level vector.
- **Senior challenge:** “How can this represent Price(4) across the protocol domain for thousands of symbols?”

## 2. Bitmap best discovery

- **Problem:** find next occupied level after best empties.
- **Choice:** one bit per ladder level plus word scan.
- **Upside:** compact and skips 64 levels per zero word.
- **Downside:** current global-end scan is O(words), with redundant consistency state.
- **Missing evidence:** sparse/wide worst-case distribution and hierarchy comparison.
- **Reversibility:** localized behind best-refresh helpers.

## 3. Intrusive list versus library containers

- **Problem:** FIFO append and arbitrary known-order cancel.
- **Choice:** `Order` owns `prev/next`; level owns head/tail observers.
- **Upside:** no separate list node and O(1) unlink.
- **Downside:** pointer/lifetime coupling and dependent traversal.
- **Alternatives:** `std::list` adds allocation/node wrapper; deque/vector improve density but complicate stable arbitrary removal; index links preserve stability with smaller handles.
- **Challenge:** cancellation is average O(1), not unconditional constant time because ID lookup and best refresh remain.

## 4. Object pool versus general allocator

- **Problem:** per-order allocation latency and stable addresses.
- **Choice:** growing chunk pool with intrusive free list.
- **Evidence:** historical pooled results suggest improvement, but narrative and saved outputs are not fully reconciled.
- **Upside:** reuse, stability, controlled order storage.
- **Downside:** growth cliff, large eager chunk, incomplete live-object destruction, hash allocation remains.
- **Alternative:** monotonic/PMR resources, pooled hash nodes, fixed-capacity arena, specialized allocator.

## 5. `unordered_map` versus open addressing

- **Problem:** exact ID lookup.
- **Choice:** standard node hash after a custom attempt reportedly regressed.
- **Good lesson:** reject an optimization that loses in the measured workload.
- **Missing evidence:** custom implementation, raw data, load factors, hash, deletion/growth policy.
- **Challenge:** the experiment rejects one implementation/configuration, not the entire family of flat tables.

## 6. Fixed-point versus floating point

- **Problem:** exact ordering and arithmetic.
- **Choice:** integer ticks.
- **Upside:** deterministic comparison and array indexing.
- **Downside:** scale absent from the type; `double` conversion helper reintroduces rounding ambiguity.
- **Challenge:** “Fixed point” is correct only when the scale and tick rules are explicit.

## 7. Single-threaded design

- **Problem:** avoid synchronization and preserve deterministic mutation order.
- **Choice:** one mutable book with no locks.
- **Upside:** simpler invariants, no contention, deterministic replay.
- **Downside:** one core per ownership domain and no concurrent access safety.
- **Scalability path:** shard symbols among single-writer workers rather than share one book.
- **Challenge:** lock-free is not automatically the next step; ownership partitioning is often cleaner.

## 8. One-symbol replay

- **Problem:** focus implementation and measurement.
- **Choice:** filter adds by eight-byte symbol; lifecycle follows ID membership.
- **Upside:** one book and small routing surface.
- **Downside:** still parses all messages, constructs a large ladder for one symbol, and cannot study cross-symbol throughput.
- **Alternative:** stock-locate directory and sharded books.

## 9. `mmap` historical ingestion

- **Problem:** sequentially access large files without an explicit read/copy buffer loop.
- **Choice:** private mapping, advice, manual page touch.
- **Upside:** concise access and clean byte span.
- **Downside:** file-cache/fault behavior, OS-specific hints, large mapping, unchecked calls.
- **Challenge:** `mmap` changes I/O mechanics; it does not make I/O free.

## 10. Raw-symbol filter

- **Problem:** avoid variable-length string comparison on adds.
- **Choice:** compare packed eight raw bytes.
- **Upside:** fixed work and alias-safe `memcpy`.
- **Downside:** parser still copies/trim strings; stock locate is ignored.
- **Evidence:** profiling narrative says `strcmp` mattered, but flamegraph artifacts are placeholders.

## 11. Cache-line-aligned orders

- **Problem:** predictable node layout and avoid split/shared lines.
- **Choice:** exactly one 64-byte line per order.
- **Upside:** stable alignment, no two orders share one line.
- **Downside:** lower density, unused fields/padding, more TLB/cache footprint.
- **Missing evidence:** controlled compact-layout comparison.

## 12. Parser/book separation

- **Problem:** keep protocol bytes out of generic book logic.
- **Choice:** normalized `Message`, then `BookReplay` adapter.
- **Upside:** useful modularity and independent parser/book testing potential.
- **Leak:** replay converts price scale incorrectly and parser error status is too weak.
- **Alternative:** type-specific messages or zero-copy views with explicit lifetime.

## 13. Reconstruction versus matching

- **Problem:** project contains both feed reconstruction and synthetic market consumption.
- **Choice:** one `OrderBook` API hosts both, while aggressive limit matching is absent.
- **Risk:** ambiguous project identity and modify priority semantics.
- **Better boundary:** distinct reconstruction mutations and matching commands, or explicitly scope out matching.

## 14. Historical replay versus live feed

- **Problem:** learn order-level feed handling deterministically.
- **Choice:** BinaryFILE replay.
- **Upside:** reproducible input, no packet loss/network setup, convenient profiling.
- **Downside:** does not teach redundancy, sequencing, recovery, backpressure, or live deadlines.
- **Challenge:** historical throughput cannot be equated with real-time feed-handler readiness.

## Decision-defense framework

For each choice say:

1. “The workload assumption was …”
2. “The mechanism is …”
3. “The repository establishes …”
4. “It does not establish …”
5. “The failure/scaling boundary is …”
6. “I would validate the next alternative by …”

