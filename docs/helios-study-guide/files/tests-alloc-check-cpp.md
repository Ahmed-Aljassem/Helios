# File Guide: `tests/alloc_check.cpp`

## 1. File purpose

Attempts to count process-wide C++ `new` calls during 100k “steady-state” Adds.

## 2. Architecture position

Standalone diagnostic source excluded from CMake; intended to validate allocation claims.

## 3. Dependencies

Order book, atomics, C allocation, and iostream.

## 4. Symbols defined

Global counter, replacements for scalar `operator new` and sized/unsized delete, `main`.

## 5. Symbols consumed

`malloc/free`, `OrderBook` constructor/Add, atomic load/increment.

## 6. Source-order walkthrough

- **Lines 7–10:** overrides some global allocation forms, incrementing an atomic then calling `malloc`. It omits array, aligned, nothrow, and other overloads relevant to a complete counter; failure should throw `bad_alloc` but this implementation returns null.
- **Lines 14–18:** constructs `OrderBook book(100000)`. By the actual signature, 100000 is `min_price`, while default max is 200000—not pool capacity. Warm-up prices 9000–11000 are all out of range and rejected.
- **Lines 20–25:** measured Adds use the same invalid prices, so they do not allocate orders or hash nodes. Return values/counts are ignored.
- **Lines 27–29:** prints a likely zero delta as if successful steady-state Adds had occurred. This is HEL-008's false experiment.

## 7. Inputs and outputs

No input. Output is allocation-count delta, but the workload validity precondition is unmet.

## 8. Ownership and lifetime

Global replacement affects compatible allocations process-wide. Book owns its containers/pool. Atomic persists for process lifetime.

## 9. Invariants

Every measured operation must be accepted; instrumentation must intercept every allocation form relevant to the claim; setup must establish intended capacities; counter reads must bracket only the target region.

## 10. Errors and edge cases

Invalid range usage, incomplete replacement set, aligned `Order` allocation behavior, null-return contract violation, allocations from I/O/runtime, compiler/linker replacement behavior.

## 11. Performance implications

Atomic increment changes allocator cost but the intended metric is count, not latency. A correct run would still observe unordered-map node allocations unless a different allocator/container policy intervenes.

## 12. Technical debt

Experiment is invalid, not built, lacks assertions and manifest, and instrumentation is incomplete.

## 13. Related findings

HEL-006, HEL-008, HEL-015, HEL-035, HEL-046–048.

## 14. Interview questions

- What does `OrderBook(100000)` mean?
- Which allocation overloads are missing?
- Why should every benchmark assert successful operations?

## 15. Exercises

Predict `getTotalOrders()` after warm-up and measurement. Design an allocator-instrumentation matrix that distinguishes pool block, map node, bucket, vector, and reporting allocations.
