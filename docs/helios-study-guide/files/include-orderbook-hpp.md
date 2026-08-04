# File Guide: `include/orderbook.hpp`

## 1. File purpose

Declares the central order-book API and defines its construction, accessors, price/index transforms, bitmap primitives, and best-bit scans inline.

## 2. Architecture position

This class is the state-machine core between replay/bench/test clients and lower-level order, level, hash, ladder, bitmap, and pool structures.

## 3. Dependencies

`PriceLevel`, `ObjectPool`, `unordered_map`, `vector`, and integer types.

## 4. Symbols defined

`OrderBook`; constructor/destructor; overloads of `addOrder`; cancel, synthetic execute, modify; state accessors; `printBook`; mapping/bitmap/search helpers; all state members.

## 5. Symbols consumed

Replay adapter, benchmarks, manual driver, and GTest consume the API. `orderbook.cpp` defines non-inline operations.

## 6. Source-order walkthrough

- **Lines 9–18:** constructor computes inclusive range, eagerly creates both ladders and bitmaps, initializes sentinel best indexes/counters/ID generator, and creates a pool. Reversed/overflowing bounds are not validated before size conversion/allocation (HEL-018).
- **Lines 19–23:** every level receives its absolute price; hash buckets reserve `pool_cap`, which reduces but does not eliminate node allocation or later rehash.
- **Lines 25–33:** default destruction follows member reverse order. Auto-ID overload post-increments even if the delegated add fails; caller-ID overload supports feed references.
- **Lines 35–37:** declarations expose cancellation, synthetic matching, and quantity modification, mixing reconstruction and simulator semantics.
- **Lines 39–50:** best price uses zero as empty sentinel; spread also returns zero for unavailable or truly locked states. `getOrder` returns a non-owning const pointer. “active” side counts are level counts, not order counts (HEL-041).
- **Line 52:** diagnostic API is implemented as no-op (HEL-039).
- **Lines 55–59:** arithmetic mapping and raw bit updates assume a validated in-range nonnegative index.
- **Lines 60–69:** best refresh scans all words from high/low ends and uses compiler builtins only on nonzero words. Complexity is O(number of bitmap words) worst case.
- **Lines 71–78:** state is deliberately redundant: bounds, levels, occupancy, best cache, active-level counts, ID map, generator/global count, and pool.

## 7. Inputs and outputs

Construction takes price domain and initial pool capacity. Mutations return ID/bool/executed quantity. Queries expose best/spread/counts/order pointer.

## 8. Ownership and lifetime

The book owns all containers and pool. Pool owns orders. Map and levels store non-owning pointers. A pointer returned by `getOrder` expires upon any operation that removes that order or book destruction.

## 9. Invariants

Use the complete I1–I19 list in [04](../04-data-structures-and-invariants.md): identity-map/queue bijection, link consistency, totals/counts, occupancy equivalence, best correctness, range/side consistency, and pool liveness.

## 10. Errors and edge cases

Invalid range, out-of-range price, zero quantity, duplicate ID, ID generator collision/overflow, empty sentinels, unsigned quantity arithmetic, map allocation failure, and non-atomic replace through adapter.

## 11. Performance implications

Direct indexing is cheap but construction/memory are range-proportional. Hash lookup and pooled node access are scattered. Bitmap scan bounds depend on range. Inline helpers inherit consumer compile flags.

## 12. Technical debt

Constructor validation, duplicate policy, zero policy, modification priority, named configuration, explicit error types, invariant validator, and clean reconstruction/simulation separation.

## 13. Related findings

HEL-002–006, HEL-009, HEL-017–021, HEL-027, HEL-035–036, HEL-039, HEL-041–042.

## 14. Interview questions

- Enumerate all state touched by Add and Cancel.
- Why is zero an ambiguous best-price sentinel?
- Is best refresh O(1)?
- What invalidates a pointer returned by `getOrder`?

## 15. Exercises

Calculate memory for default and replay ranges using `sizeof(PriceLevel)`. Prove bitmap/best invariants for every empty/non-empty transition. Design a typed constructor result without implementing it.
