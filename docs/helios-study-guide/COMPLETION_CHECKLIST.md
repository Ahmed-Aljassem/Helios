# Study Guide Creation Plan and Completion Checklist

## Implementation plan

### Stage A — Grounding and navigation

- [x] Reconcile the 48 Git-tracked files.
- [x] Reconcile the `HEL-001`–`HEL-048` audit ledger.
- [x] Inspect `ENGINEERING_BACKLOG.md` and preserve its priorities and terminology.
- [x] Establish evidence labels and cross-link conventions.
- [x] Create the guide navigation and this checklist.

### Stage B — Mental model and architecture

- [x] Complete learning map and prerequisite graph.
- [x] Complete first-principles market/book tutorial.
- [x] Complete architecture, dependency, ownership, build, and data-flow diagrams.
- [x] Complete all requested end-to-end execution traces.

### Stage C — Systems deep dives

- [x] Complete data-structure and invariant course.
- [x] Complete ownership/lifetime/allocation course.
- [x] Complete CPU/cache/TLB/branch/allocator course.
- [x] Complete ITCH/BinaryFILE course using official primary sources.
- [x] Complete benchmark and measurement course.
- [x] Complete testing/reference-model course.
- [x] Complete CMake/build/tooling course.

### Stage D — Judgment and professional defense

- [x] Complete engineering decision/tradeoff catalog.
- [x] Explain every `HEL-001`–`HEL-048` finding educationally.
- [x] Complete production evolution exercise.
- [x] Complete oral-exam and hostile-interviewer defense guide.
- [x] Complete glossary.
- [x] Complete 7-, 30-, and 90-day study plans.

### Stage E — Source walkthroughs

- [x] Create one guide for every tracked header/source file.
- [x] Create one guide for every benchmark and test file.
- [x] Create guides for CMake, `.gitignore`, and the replay script.
- [x] Map every tracked documentation/evidence artifact to a shared chapter.

### Stage F — Verification

- [x] Verify all required textbook documents exist.
- [x] Verify every code/build/test/benchmark/script file has a per-file guide.
- [x] Verify all 48 tracked files appear in the coverage matrix.
- [x] Verify every `HEL-001` through `HEL-048` appears in the debt chapter.
- [x] Check relative Markdown links where feasible.
- [x] Confirm only documentation under this directory was created during this task.

Verification result: 17 numbered chapters plus navigation/checklist/coverage files exist; 23 dedicated per-file guides each contain all 15 required sections; 48 tracked paths are mapped; 48 distinct finding IDs are present; all local Markdown file targets resolve. The pre-existing untracked `ENGINEERING_BACKLOG.md` was read but not modified as part of the study-guide creation.

## Definition of complete

The study guide is complete only when a reader can:

- trace an ITCH add, reduction, deletion, execution, and replacement from bytes to final pointers and counters;
- state and check the complete book invariant set;
- explain all owners and non-owning pointers;
- distinguish saved measurements from inference;
- explain why the current price conversion is incorrect;
- explain exactly what the 26.6 ns artifact does and does not establish;
- describe a verification oracle and a production evolution;
- defend limitations without becoming evasive;
- locate every tracked file in the coverage matrix.

## Writing discipline

- Shared concepts live in numbered textbook chapters.
- Per-file guides explain how that file realizes the concepts and link back rather than repeating entire tutorials.
- Historical results remain historical unless tied to source/build/hardware provenance.
- Defects are taught as state-machine, ownership, protocol, or measurement lessons.
- Proposed redesigns are educational exercises, not claims about any trading firm.
