# File Guide: `include/price_level.hpp`

## 1. File purpose

Declares one price bucket: its price, FIFO endpoints, aggregate quantity, order count, and mutation primitives.

## 2. Architecture position

Every element in both direct ladders is a `PriceLevel`; it is a non-owning intrusive container over pooled orders.

## 3. Dependencies

`order.hpp`, domain types, and size/integer headers.

## 4. Symbols defined

`PriceLevel`; constructor; accessors; `addOrder`, `removeOrder`, `reduceHeadQuantity`, `adjustTotalQuantity`.

## 5. Symbols consumed

`OrderBook` constructs, indexes, queries, and mutates levels. Tests call its interface indirectly/current stub directly not active.

## 6. Source-order walkthrough

- **Lines 9–11:** constructor produces an empty level at price zero; `OrderBook` later assigns the actual ladder price to every element.
- **Lines 13–19:** setters/accessors expose cached fields and raw non-owning endpoints. `isEmpty` trusts `head_`; correctness also requires tail/count/quantity agreement.
- **Lines 21–24:** mutation declarations divide queue topology, head reduction, and signed aggregate adjustment. Their implementations and caller ordering jointly enforce invariants.
- **Lines 26–31:** five fields are redundant views of the queue and must move together.

## 7. Inputs and outputs

Mutators accept an `Order*`, a reduction quantity, or signed delta; accessors return current cached state.

## 8. Ownership and lifetime

The level owns no orders. Head/tail/links are valid only until `OrderBook` unlinks then returns an object to the pool.

## 9. Invariants

Empty iff head and tail are null, count zero, aggregate zero. Non-empty head.prev and tail.next are null. Bidirectional links agree. Count and quantity equal traversal results. Every order price/side matches the containing ladder slot.

## 10. Errors and edge cases

Removing a foreign/unlinked pointer, reducing an empty head, reducing too much, or applying a negative delta larger than total can underflow unsigned state. The API assumes trusted callers.

## 11. Performance implications

Append/unlink are fixed pointer operations. Traversal is pointer-chasing. Cached aggregate/count make reads cheap at the cost of multi-field update risk.

## 12. Technical debt

Preconditions are unenforced at this boundary; public head pointers expose internals; signed-to-unsigned aggregate adjustment deserves checked semantics.

## 13. Related findings

HEL-002, HEL-003, HEL-004, HEL-009, HEL-021, HEL-034, HEL-042.

## 14. Interview questions

- Prove removal for singleton, head, tail, and middle nodes.
- Which level field is canonical?
- Why is raw-pointer exposure risky even when `const Order*` is returned elsewhere?

## 15. Exercises

Write before/after tables for all four unlink shapes. Construct one corruption each that fools `isEmpty`, count, or aggregate alone.
