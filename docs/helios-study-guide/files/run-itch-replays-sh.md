# File Guide: `run_itch_replays.sh`

## 1. File purpose

Runs `itch_book_replay` over the Cartesian product of input captures and symbols, teeing console output into a timestamped Markdown report.

## 2. Architecture position

Experiment orchestration around the replay executable and tracked report artifacts; not part of runtime parsing/book logic.

## 3. Dependencies

Bash, `getopts`, `date`, `mkdir`, `tee`, `du`, `cut`, `basename`, executable replay binary, and local ITCH files.

## 4. Symbols defined

Shell variables, `usage` function, `FILES`/`SYMBOL_ARR` arrays, loop variables, and report schema.

## 5. Symbols consumed

CLI arguments, home-directory default data path, filesystem, replay exit/output, Bash `SECONDS` and `PIPESTATUS`.

## 6. Source-order walkthrough

- **Lines 1–11:** Bash shebang and `set -uo pipefail`; omission of `-e` allows continued reporting after failures by design/accident. Defaults identify symbol, docs output, and home data.
- **Lines 13–30:** here-document documents option-before-files constraint and auto-discovery.
- **Lines 32–44:** `getopts` fills settings and rejects bad/missing options.
- **Lines 46–56:** searches three relative executable candidates and validates executable bit, but does not identify source revision/hash.
- **Lines 58–71:** explicit arguments are preserved as an array; otherwise a nullglob safely discovers capture suffixes. Individual explicit files are validated later.
- **Lines 73–75:** removes literal spaces and comma-splits symbols; tabs/case/length/domain are not validated.
- **Lines 77–82:** creates output directory and chooses second-resolution name; explicit outfile parent need not equal/create `DOCS_DIR`.
- **Lines 84–93:** truncates output via `tee` and records date, binary path, symbols, file count—insufficient provenance.
- **Lines 95–110:** skips missing files, computes human size through a pipeline, then emits a section per file/symbol.
- **Lines 112–124:** runs binary with quoted args, merges stderr, tees output, correctly captures producer status from `PIPESTATUS[0]`, and records elapsed whole seconds. It increments run count even on failure and continues.
- **Line 128:** final message does not aggregate failed-run status; script exits with last `tee` status, potentially success despite replay failures.

## 7. Inputs and outputs

Options/files plus environment/filesystem enter. A Markdown report and terminal output leave; no source/input/compiler/hardware hashes are recorded.

## 8. Ownership and lifetime

Shell process owns variables/arrays; replay child owns each run; `tee` owns open report descriptors per pipeline. Input captures are borrowed read-only.

## 9. Invariants

Binary executable and every run input valid; report section closes even on failure; child status preserved; unique report path; manifest identifies experiment; final script status reflects failures.

## 10. Errors and edge cases

Empty symbol entries, >8-byte symbol truncation downstream, output collision within a second, unwritable explicit parent, tool absence, `du` failure, partial reports, nonzero child hidden by final success.

## 11. Performance implications

Script timing includes process startup, book construction, replay, reporting, and cleanup. Sequential product runs can warm filesystem cache, biasing later comparisons. `tee` affects only outside replay's internal phase timers but affects wall elapsed.

## 12. Technical debt

Incomplete failure aggregation/provenance, no checksum/build/hardware manifest, cache-state ordering bias, weak symbol validation, historical report traceability.

## 13. Related findings

HEL-005, HEL-007, HEL-014, HEL-016, HEL-023, HEL-025–026, HEL-033, HEL-043, HEL-048.

## 14. Interview questions

- Why is `PIPESTATUS[0]` necessary after `tee`?
- Can the script finish with status zero after a failed replay?
- How does run order affect page-cache state?

## 15. Exercises

Trace shell exit status through every failure path. Design a manifest schema and a randomized/cold-warm run schedule without editing the script.
