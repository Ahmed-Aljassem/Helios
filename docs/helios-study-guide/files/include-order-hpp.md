# File Guide: `include/order.hpp`

## 1. File purpose

Defines the stable-address order record used simultaneously as market state and an intrusive FIFO node.

## 2. Architecture position

`Order` connects pool storage, ID-map lookup, and `PriceLevel` queues. See [04](../04-data-structures-and-invariants.md) and [05](../05-memory-ownership-and-lifetimes.md).

## 3. Dependencies

Domain types from `types.hpp`; `<iostream>` for the cold diagnostic method.

## 4. Symbols defined

`alignas(64) struct Order`, two constructors, `print`, and size/alignment static assertions.

## 5. Symbols consumed

`PriceLevel`, `OrderBook`, `ObjectPool<Order>`, replay inspection, tests, and benchmarks.

## 6. Source-order walkthrough

- **Lines 8–19:** layout places identity, price, quantity, and intrusive pointers first; timestamp/side/type follow. Comments label hot/cold use, but no counter evidence proves this split. Padding plus alignment makes each object 64 bytes.
- **Lines 21–25:** primary constructor initializes every link to null, a necessary precondition before first insertion.
- **Lines 27–30:** default constructor creates sentinel-like values but a real default order is not automatically valid/live.
- **Lines 32–37:** diagnostic formatting reads fields and writes/flushed output; `std::endl` can be expensive and is strictly cold-path.
- **Lines 40–41:** compile-time ABI assertions deliberately reject platforms/layouts where the cache-line assumption fails.

## 7. Inputs and outputs

Constructors accept the complete resting-order record. `print` emits human-readable state.

## 8. Ownership and lifetime

The pool owns live `Order` objects. `next`/`prev` and the hash map are non-owning. Links are valid only while all referenced orders remain live and in the same queue.

## 9. Invariants

A live order has unique ID, positive remaining quantity under the desired contract, in-range price, one side, exactly one queue, and links consistent with level head/tail. A free slot is not an `Order`.

## 10. Errors and edge cases

Public fields permit arbitrary mutation and link corruption. A copied `Order` would copy topology pointers. The type does not itself track live/free state.

## 11. Performance implications

One-order-per-line may reduce cross-object sharing but doubles footprint relative to a compact record in plausible layouts. Random cancellation still incurs dependent loads; alignment does not ensure adjacency in reuse order.

## 12. Technical debt

Unproven alignment, public invariant-bearing fields, I/O in the core record, and unused/stale timestamp/type semantics.

## 13. Related findings

HEL-015, HEL-038, HEL-042, HEL-044.

## 14. Interview questions

- What problem does 64-byte alignment solve in a single-writer design?
- Why must addresses remain stable?
- Would splitting links/metadata improve cache behavior?

## 15. Exercises

Draw exact offsets and padding. Design two compact alternative layouts and predict which fields each operation touches before proposing measurement.
