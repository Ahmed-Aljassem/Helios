# File Guide: `.gitignore`

## 1. File purpose

Declares generated build products, editor state, profiler outputs, and local data that should not become source history.

## 2. Architecture position

It surrounds the build/evidence workflow rather than the runtime graph. See [10](../10-build-system-and-tooling.md).

## 3. Dependencies

Git consumes it; CMake, compilers, profilers, and editors produce the paths it classifies.

## 4. Symbols defined

None. Patterns are the file's semantic units.

## 5. Symbols consumed

None; it names generated directories/files from the toolchain.

## 6. Source-order walkthrough

The blocks exclude CMake build trees and compiled objects/libraries/executables, then IDE/OS metadata, profiling outputs, downloaded ITCH captures, and local result files. Each block answers whether an artifact is reproducible source, compact evidence worth curating, or disposable machine state. The rules coexist with tracked historical outputs under `docs/`; already tracked files remain tracked even if a later pattern matches them.

## 7. Inputs and outputs

Input is a candidate workspace path; output is Git's ignored/not-ignored classification. It does not delete data.

## 8. Ownership and lifetime

Git owns index decisions; ignored files remain owned by their creating tool/user on disk.

## 9. Invariants

No required source or small reproducibility manifest should be ignored. Large captures, caches, and rebuildable binaries should not be committed accidentally.

## 10. Errors and edge cases

Broad patterns can hide useful manifests; missing patterns can expose gigabyte captures. Ignore rules do not affect a file already in the index.

## 11. Performance implications

No runtime effect. Repository size and status-scan noise affect developer workflow.

## 12. Technical debt

Artifact policy is not fully coherent with numerous tracked historical outputs (HEL-016, HEL-048).

## 13. Related findings

HEL-014, HEL-016, HEL-043, HEL-048.

## 14. Interview questions

- Why might benchmark summaries be tracked while raw captures are not?
- What fields make a small result artifact reproducible?

## 15. Exercises

Classify every generated path as source, manifest, raw evidence, derived summary, cache, or opaque binary. Propose retention rules without changing this file.
