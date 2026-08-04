# File Guide: `include/book_replay.hpp`

## 1. File purpose

Adapts decoded ITCH order-lifecycle messages into mutations of one target-symbol `OrderBook` and exposes event counters.

## 2. Architecture position

It is the semantic boundary between protocol representation and the generic local book.

## 3. Dependencies

`orderbook.hpp`, `itch_parser.hpp`, byte-copy/string length utilities.

## 4. Symbols defined

`BookReplay`; constructor; `onMessage`; counter accessors; `toCents`, `isTarget`, `onAdd`, `onDelete`, `onReduce`, `onReplace`; state members.

## 5. Symbols consumed

`itch_book_replay.cpp` creates it and passes `onMessage` as parser callback. It consumes decoded `itch::Message` and `OrderBook` APIs.

## 6. Source-order walkthrough

- **Lines 7–13:** comments define one-symbol strategy and claim an observed comparison cost; that percentage is historical and lacks a complete manifest.
- **Lines 16–23:** target string is space-padded/truncated to eight bytes and copied into `uint64_t`. `strlen` requires a valid null-terminated pointer. `memcpy` avoids alignment/aliasing violations; numeric value is host-endian but equality of identically copied bytes is sufficient locally.
- **Lines 24–33:** dispatches A/F, D, X, E/C, U. `seen_` counts every modeled callback delivered, including non-target or missing-ID events.
- **Lines 34–38:** counters represent accepted-ish adapter actions with subtle differences; they are not a complete outcome taxonomy.
- **Line 40:** divides Price(4) by 100, critically merging legal levels (HEL-001).
- **Lines 41–45:** eight raw symbol bytes are copied to a local integer for equality.
- **Lines 46–50:** only target Adds establish tracked IDs; any side other than B becomes SELL, and successful book result increments `added_`.
- **Lines 51–59:** delete checks existence before cancel; reduce performs read-then-modify/cancel and counts either partial/full reduction. Quantity comparison widens shares.
- **Lines 61–67:** replace reads old side, cancels old ID, then adds new ID/price/qty. If add fails, old state is already lost; counter increments only on add success.
- **Lines 68–70:** non-owning book reference must outlive adapter; counters are local metadata.

## 7. Inputs and outputs

Input is a borrowed book, target C string, and synchronous decoded messages. Output is book mutation and counters; no detailed anomaly result is returned.

## 8. Ownership and lifetime

The caller owns `OrderBook`; `BookReplay` borrows it. Each message reference is valid only during callback. Target bytes and counters are owned by adapter.

## 9. Invariants

Only target-symbol Adds enter the book; later lifecycle events affect only IDs already admitted. Replacement should be atomic as a logical event. Feed price ordering must be preserved—currently violated.

## 10. Errors and edge cases

Null symbol, longer-than-eight truncation, invalid side, duplicate/new ID, out-of-range/zero values, cancel/execute exceeding remaining, replace add failure, and feed-reference anomaly.

## 11. Performance implications

One switch per message; Add pays eight-byte copy/compare; lifecycle events pay hash lookup, sometimes twice (`getOrder` then mutation lookup). Price conversion is cheap but wrong. Counters add writes.

## 12. Technical debt

Price scale, configured range, result taxonomy, locate-based identity, metadata loss, non-atomic replace, double lookups, and reconstruction/simulation semantics.

## 13. Related findings

HEL-001–003, HEL-012, HEL-017, HEL-022–024, HEL-037–038, HEL-042.

## 14. Interview questions

- Why can non-Add messages be filtered by ID?
- Show how two raw prices merge.
- What if replacement's new Add fails?
- What exactly does `seen()` mean?

## 15. Exercises

Trace one target and one non-target order through identical lifecycle messages. Design a typed outcome enum and counter taxonomy. Draw a failure-atomic replace transaction.
