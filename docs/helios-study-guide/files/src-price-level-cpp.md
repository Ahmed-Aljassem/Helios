# File Guide: `src/price_level.cpp`

## 1. File purpose

Implements FIFO append/unlink and aggregate-quantity maintenance for `PriceLevel`.

## 2. Architecture position

This is the queue primitive underneath every ladder mutation. `OrderBook` is its trusted orchestrator.

## 3. Dependencies

The declaration and `Order` layout through `price_level.hpp`.

## 4. Symbols defined

`PriceLevel::addOrder`, `removeOrder`, `reduceHeadQuantity`, `adjustTotalQuantity`.

## 5. Symbols consumed

`Order` link/quantity fields and all private `PriceLevel` state.

## 6. Source-order walkthrough

- **Lines 5–20, Add:** null is a no-op. New order points back to old tail and forward to null. Old tail points forward, or head is initialized for an empty list. Tail becomes new order; aggregate and count increase. Preconditions such as “not already linked” and matching price are trusted.
- **Lines 22–45, Remove:** cache both neighbors; redirect predecessor or head, then successor or tail; subtract the removed order's entire remaining quantity and decrement count; clear its links. This covers singleton/head/tail/middle through two independent endpoint branches.
- **Lines 47–56, Reduce head:** rejects empty or over-reduction. Subtracts from head and total, then calls `removeOrder` at zero. Because the order's quantity is already zero, `removeOrder` subtracts zero—correct but subtle.
- **Lines 58–64, Adjust total:** positive delta adds; negative delta uses `-(delta + 1) + 1` to avoid negating `INT64_MIN`. It still trusts that subtraction cannot underflow total.

## 7. Inputs and outputs

Inputs are borrowed order pointers/quantities/deltas. Output is in-place topology and cached metadata mutation; no status communicates invalid non-null pointers.

## 8. Ownership and lifetime

No allocation or destruction. Callers must keep orders alive while linked and deallocate only after unlink.

## 9. Invariants

After each call, endpoint links, bidirectional neighbor links, count, and aggregate must match traversal. An order is either unlinked or belongs to exactly one level.

## 10. Errors and edge cases

Double removal underflows count/total and can corrupt neighbors. Foreign links splice lists. Zero-quantity Add creates non-progress state. Quantity/delta overflow is unchecked.

## 11. Performance implications

Append/unlink use a small fixed set of writes. Random order addresses may miss cache. Aggregate caching avoids scans. A full head removal through `reduceHeadQuantity` touches the level twice logically but not asymptotically.

## 12. Technical debt

No debug assertions, no membership identity, silent invalid input, unsigned overflow policy, and semantics shared between generic modification and execution.

## 13. Related findings

HEL-002–004, HEL-009, HEL-021, HEL-034.

## 14. Interview questions

- Why does full `reduceHeadQuantity` not double-subtract quantity?
- Prove singleton removal.
- What prevents one order joining two levels?

## 15. Exercises

Annotate every pointer write in four removal shapes. Invent a debug-only validator that detects cycles, wrong endpoints, count mismatch, and aggregate mismatch.
