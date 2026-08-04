# File Guide: `benchmarks/itch_replay.cpp`

## 1. File purpose

Minimal mmap-backed parser smoke/replay program that counts modeled ITCH message types.

## 2. Architecture position

Exercises BinaryFILE framing/decoding without constructing an order book.

## 3. Dependencies

Parser header and POSIX file/stat/mmap APIs plus stdio.

## 4. Symbols defined

Only `main`.

## 5. Symbols consumed

`open`, `fstat`, `mmap`, `ITCHParser::parseBuffer`, `munmap`, `close`.

## 6. Source-order walkthrough

- **Lines 1–8:** labels itself a sketch; includes an unused initializer-list header.
- **Lines 10–13:** dereferences `argv[1]` without checking `argc`; ignores open/fstat/mmap errors and casts `MAP_FAILED` as if data.
- **Lines 15–18:** zeroes 128 counters; callback masks type with 127, then increments only for messages decoder labels modeled. `n` equals sum of those callbacks.
- **Lines 20–22:** reports total and seven supported letter buckets.
- **Lines 24–26:** unmaps and closes without checking; zero/invalid sizes are not handled deliberately.

## 7. Inputs and outputs

Input is path argument and mapped file bytes. Output is modeled message counts.

## 8. Ownership and lifetime

OS owns file object behind fd; process owns descriptor and mapping handles. Parser borrows mapping for the call. Callback message is stack-local and not retained.

## 9. Invariants

Descriptor/stat/mapping must be valid before parsing; mapping remains live throughout; unmap/close occur once; count labels match decoder taxonomy.

## 10. Errors and edge cases

Missing argument, failed calls, directory/empty/huge file, truncated frame, zero frame, unsupported valid types, malformed supported types. Parser silently stops on several conditions.

## 11. Performance implications

Page faults and decoder work are mixed; callback counter array is cache-resident. No timing is performed. `mmap` avoids an explicit read/copy loop but does not guarantee residency.

## 12. Technical debt

Production-grade resource/error handling, exact status counters, portable mapping wrapper, tests, and clearer “sketch” scope.

## 13. Related findings

HEL-011–012, HEL-022–023, HEL-026, HEL-037–038, HEL-046.

## 14. Interview questions

- What happens when `mmap` returns `MAP_FAILED`?
- What does “modeled messages” exclude?
- Does mapping read the file immediately?

## 15. Exercises

Enumerate every syscall result that must be checked. For a file with 100 frames, classify how `n` can be less than 100.
