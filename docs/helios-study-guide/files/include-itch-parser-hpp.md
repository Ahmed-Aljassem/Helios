# File Guide: `include/itch_parser.hpp`

## 1. File purpose

Header-only BinaryFILE framer and partial Nasdaq TotalView-ITCH 5.0 order-message decoder.

## 2. Architecture position

It is the untrusted-byte boundary. Parser-only replay observes its output; `BookReplay` consumes modeled messages to mutate a book.

## 3. Dependencies

Fixed-width/size types and `memcpy`. External semantic dependency: official Nasdaq ITCH 5.0 and BinaryFILE specifications.

## 4. Symbols defined

`rdU16`, `rdU32`, `rdU48`, `rdU64`, `trimStock`, `itch::Message`, `decode`, and templated `parseBuffer`.

## 5. Symbols consumed

Replay executables and `book_replay.hpp`; callbacks consume ephemeral `const Message&`.

## 6. Source-order walkthrough

- **Lines 1–13:** explicitly rejects struct overlay and announces specification dependence.
- **Lines 15–26:** big-endian readers shift bytes into host integers. They assume callers established enough remaining bytes.
- **Lines 28–30:** trims only trailing ASCII spaces from a nine-byte null-terminated copy.
- **Lines 32–44:** one superset struct stores common fields for all modeled types. Default values make absent fields indistinguishable from legitimate zero unless type is consulted. `priceDollars` is display-only floating conversion.
- **Lines 46–52:** `decode` resets output and sets type; `false` conflates malformed supported and valid unsupported messages.
- **Lines 53–72:** A/F require minimum 36/40 bytes; decode timestamp at 5, ref 11, side 19, shares 20, stock 24, price 32. F's MPID bytes are not retained.
- **Lines 73–85:** E/C decode execution shares; C decodes printable execution price at offset 32 but replay reduces the resting order rather than changing its level.
- **Lines 86–104:** X cancel shares, D full delete, U old ref/new ref/shares/new price.
- **Lines 105–108:** unknown types optionally expose timestamp and return false.
- **Lines 111–125:** parse loop reads two-byte frame length, treats zero/truncated tail as silent termination, calls callback only for modeled decode success, and advances by declared frame length.

## 7. Inputs and outputs

Raw pointer plus byte count enters `parseBuffer`; synchronous callback receives decoded stack object; return value counts modeled callbacks, not all frames.

## 8. Ownership and lifetime

The caller owns buffer bytes. `Message` is stack-local per frame. Callback reference expires when invocation returns; storing it is invalid. Stock arrays are copied into the message.

## 9. Invariants

No read beyond frame; offset schema matches type; integer values preserve wire bits; outer offset advances exactly prefix plus payload; callback fires only for classified decoded messages.

## 10. Errors and edge cases

Null data with nonzero size, exact versus minimum lengths, invalid side/domain, zero frame, truncation, unknown type, trailing bytes, oversized logical values, and lack of detailed status.

## 11. Performance implications

Explicit shifts are compiler-friendly and avoid unaligned object UB. One `Message` reset/copies stock per supported Add. Header-only code optimizes in each consumer. Branch distribution follows feed type mix.

## 12. Technical debt

Status taxonomy, exact length/field validation, stock locate/tracking retention, session completeness, parser tests/fuzzing, and callback contract documentation.

## 13. Related findings

HEL-001, HEL-007, HEL-011–012, HEL-022–024, HEL-037–038.

## 14. Interview questions

- Why not cast a packed C++ struct over bytes?
- What does `parseBuffer` count?
- How would you distinguish unsupported from malformed?
- Why is a stack callback message safe only synchronously?

## 15. Exercises

Write an offset table for all seven types from memory. Hand-decode a U frame. Enumerate every possible terminal state of the framing loop and the counter it should affect.
