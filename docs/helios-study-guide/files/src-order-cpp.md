# File Guide: `src/order.cpp`

## 1. File purpose

Tracked placeholder for a possible out-of-line `Order` implementation.

## 2. Architecture position

None in the current build; all `Order` behavior is inline in `include/order.hpp`.

## 3. Dependencies

None.

## 4. Symbols defined

None.

## 5. Symbols consumed

None.

## 6. Source-order walkthrough

Line 1 contains only `//NOT DONE`. It produces no declaration, definition, or object code and is excluded from `SOURCES` in CMake.

## 7. Inputs and outputs

None.

## 8. Ownership and lifetime

None.

## 9. Invariants

No runtime invariant. Repository documentation should not imply this file implements `Order`.

## 10. Errors and edge cases

Accidentally adding it to a target is harmless today but signals unfinished scope.

## 11. Performance implications

None. Moving inline definitions here could change inlining and translation-unit optimization in the future.

## 12. Technical debt

The placeholder should eventually be intentionally removed or assigned a purpose; no code change is made by this guide.

## 13. Related findings

HEL-011, HEL-016, HEL-047.

## 14. Interview questions

- Why is this file not compiled?
- When should a small method be inline versus out-of-line?

## 15. Exercises

Use CMake and the linker model to explain why a tracked `.cpp` file has no effect unless a target includes it.
