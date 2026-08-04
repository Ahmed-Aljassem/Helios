# File Guide: `include/object_pool.hpp`

## 1. File purpose

Provides growing, fixed-slot storage with stable addresses and an intrusive free list.

## 2. Architecture position

`OrderBook` owns `ObjectPool<Order>`; the pool owns order bytes while map and level structures observe live objects. See [05](../05-memory-ownership-and-lifetimes.md).

## 3. Dependencies

Size/byte types, `vector`, `unique_ptr`, placement new, and perfect forwarding utilities.

## 4. Symbols defined

Template `ObjectPool<T>`; constructor; deleted copy operations; `allocate`, `deallocate`; counters; union `Slot`; `addChunk`.

## 5. Symbols consumed

`OrderBook` instantiates `ObjectPool<Order>` and calls allocation/deallocation around lifecycle changes.

## 6. Source-order walkthrough

- **Lines 11–16:** comments promise O(1)/no malloc too broadly; growth remains and client containers allocate.
- **Lines 18–26:** constructor normalizes zero chunk size and immediately adds at least one chunk. This writes free-list links across every slot.
- **Lines 28–30:** copying is prohibited because raw pointers and unique block ownership cannot be naively duplicated.
- **Lines 32–43:** `allocate` grows if empty, pops the free head, increments `allocated_`, then placement-constructs `T`. If construction throws, the slot/count are not rolled back—another generic exception-safety limitation.
- **Lines 45–53:** `deallocate` ignores null, invokes the destructor, reinterprets storage-start as `Slot`, pushes it to the free list, and decrements count. It trusts provenance and single deallocation.
- **Lines 55–58:** counters are metadata, not independent proof of reachable live objects.
- **Lines 60–66:** union overlays object storage and `next`; only the member appropriate to current lifetime may be used.
- **Lines 68–79:** `addChunk` heap-allocates an array, threads every slot onto the old free list, increases capacity, then moves ownership into the vector. If `chunks_.push_back` throws after links point into the local block, those pointers dangle.
- **Lines 81–85:** vector owns each `Slot[]`; `free_head_` is non-owning; sizes track policy/state.

## 7. Inputs and outputs

Capacity policy enters construction. Allocation arguments construct `T` and return pointer; deallocation consumes a valid live pointer.

## 8. Ownership and lifetime

`unique_ptr<Slot[]>` owns raw blocks. A slot alternates between free-list `Slot` state and live `T` lifetime. Pool teardown releases blocks but does not enumerate/destruct still-live non-trivial objects.

## 9. Invariants

Every slot is exactly free or live; free slots are reachable once from `free_head_`; `allocated + available == capacity`; live pointers belong to a retained chunk; no live slot is on free list.

## 10. Errors and edge cases

Foreign pointer, double free, construction throw, vector-growth throw, counter underflow, and teardown with live non-trivial objects. Thread safety is absent by design.

## 11. Performance implications

Warm pop/push is a short dependent pointer path. Chunk growth allocates and touches all slots. LIFO reuse improves temporal locality; blocks plus 64-byte orders affect pages/TLB.

## 12. Technical debt

Generic lifetime contract, exception rollback, broad “no malloc” comments, no membership/debug checks, and unbounded growth.

## 13. Related findings

HEL-006, HEL-013, HEL-021, HEL-035, HEL-044.

## 14. Interview questions

- When does placement new begin lifetime?
- Why does union reuse require discipline?
- What happens if `T`'s constructor throws?
- Does the pool's destructor call live `T` destructors?

## 15. Exercises

Trace three slots through acquire/release order. Draw exception-safe `addChunk` commit sequencing conceptually. Instantiate mentally with a destructor-counting `T`.
