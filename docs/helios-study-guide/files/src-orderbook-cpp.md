# File Guide: `src/orderbook.cpp`

## 1. File purpose

Implements all non-inline book mutations: Add, Cancel, synthetic market sweep, quantity Modify, and the diagnostic stub.

## 2. Architecture position

This is the principal state transition implementation. Read alongside [03](../03-end-to-end-execution-flows.md) and [04](../04-data-structures-and-invariants.md).

## 3. Dependencies

`orderbook.hpp` and `std::min`.

## 4. Symbols defined

`OrderBook::addOrder(side, price, quantity, id)`, `cancelOrder`, `executeMarketOrder`, `modifyOrder`, `printBook`.

## 5. Symbols consumed

All private mapping/bitmap/state members, `PriceLevel` mutators, pool allocation, and `unordered_map` operations.

## 6. Source-order walkthrough

### Lines 6–30 — Add

Range rejection returns ID zero. Price becomes ladder index. Pool allocation constructs an order; `orders_[id] = o` inserts or overwrites a mapped pointer. That ordering causes HEL-002 on duplicates and can throw after pool allocation (HEL-021). Side selects ladder. `wasEmpty` gates bit/active-level updates; better index updates best cache. Finally global count increments. Quantity zero and invalid enum values are not rejected; non-BUY is treated as ask.

### Lines 32–56 — Cancel

One map lookup rejects missing ID. Stored price produces index; stored side selects level. Intrusive removal updates level; when it becomes empty the bit/count are cleared and best is rescanned only if this was best. Map entry is erased before pool destruction; global count decrements. Correctness assumes stored pointer/links/range are valid.

### Lines 58–118 — Synthetic market execution

`remaining` tracks unmet aggressive quantity. A BUY consumes best asks; SELL mirrors best bids. At each level, `fill` is capped by cached aggregate. Inner loop reads head, caps trade, and either directly erases/unlinks/deallocates a full order or reduces the head. `filled` drives progress; after a level empties its bitmap/count/best are updated. The `filled == 0` break prevents infinite looping under inconsistent/zero state, but a zero-quantity queued head exposes HEL-003. Return is requested minus remaining. This method decides local matches and is not ITCH reconstruction.

### Lines 120–130 — Modify

Lookup rejects missing ID. Difference is computed by first narrowing both unsigned quantities to `int64_t`; values above `INT64_MAX` are unsafe/implementation-sensitive. The order changes in place and level aggregate adjusts. Zero remains linked, and increases keep FIFO priority (HEL-003/004).

### Lines 132–135 — Print

Consumes `depth` only to silence warnings and emits nothing.

## 7. Inputs and outputs

Mutation inputs are IDs, side, price, quantity. Status is encoded inconsistently as zero ID, boolean, or filled quantity. State changes are synchronous.

## 8. Ownership and lifetime

Add starts pool object lifetime; queue/map borrow pointer. Cancel/full execution erase all observations before pool deallocation. Returned lookup pointers can be invalidated by these paths.

## 9. Invariants

Every accepted operation must preserve the map↔queue bijection, queue topology, aggregate/count, bitmap/cache, global count, range/side correspondence, and pool live count. Add currently violates this for duplicates.

## 10. Errors and edge cases

Duplicate ID, zero quantity, out-of-range price, invalid side, allocation exceptions, huge unsigned quantity, missing ID, over-reduction via adapter semantics, stale pointers, empty/corrupt levels, and ID zero ambiguity.

## 11. Performance implications

Ordinary Add: range arithmetic, pool pop, hash node insertion/lookup, level writes, possible bitmap/cache writes. Cancel: hash dependent loads, scattered order, fixed unlink, possible word scan. Long sweeps serialize through head pointers and hash erases. Growth/rehash are slow cliffs.

## 12. Technical debt

Critical duplicate transaction, validation, modification semantics, exception safety, repeated bid/ask code, result/anomaly types, debug invariant hook, and separation of matching simulator.

## 13. Related findings

HEL-002–006, HEL-009, HEL-017–021, HEL-027–029, HEL-033, HEL-035–036, HEL-039, HEL-041–042.

## 14. Interview questions

- Why is map assignment before queue insertion still unsafe?
- List every Add allocation point.
- Why is Cancel not strict worst-case O(1)?
- How does a sweep maintain FIFO and aggregate state?
- What happens when quantity becomes zero through Modify?

## 15. Exercises

Trace duplicate Add and mark the unreachable node. Build a failure table for every Add statement. Derive exact memory writes for a full head fill versus partial fill. Refactor the bid/ask logic on paper without changing semantics.
