# File Guide: `tests/test_price_level.cpp`

## 1. File purpose

Placeholder for direct `PriceLevel` tests.

## 2. Architecture position

Would isolate intrusive queue correctness below `OrderBook`, but currently has no build role.

## 3. Dependencies

None currently.

## 4. Symbols defined

None.

## 5. Symbols consumed

None.

## 6. Source-order walkthrough

Line 1 is only `//NOT DONE`. Singleton/head/tail/middle transitions and aggregate/count behavior therefore lack isolated tests.

## 7. Inputs and outputs

None.

## 8. Ownership and lifetime

None currently. A future test must make clear that levels borrow stack/pool orders rather than own them.

## 9. Invariants

Endpoint nullness, bidirectional links, FIFO append, count, aggregate, and unlinked removed nodes are the intended test properties.

## 10. Errors and edge cases

Null mutation, foreign/double removal, zero quantity, over-reduction, singleton and transition-to-empty.

## 11. Performance implications

No current effect. Isolated tests enable safe optimization of the true constant-pointer primitive.

## 12. Technical debt

Tracked empty test and no registered target.

## 13. Related findings

HEL-003–004, HEL-009, HEL-011, HEL-016, HEL-046.

## 14. Interview questions

- Which queue cases form a complete decision table?
- Should `PriceLevel` reject foreign nodes or rely on its owner?

## 15. Exercises

Create the full state table for add/remove/reduce, including expected head/tail/link/count/quantity after each case.
