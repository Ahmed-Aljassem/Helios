# Technical Debt and Limitations

This chapter turns the audit ledger into lessons. Severity describes the present repository, not the author's ability. A small experimental system can legitimately omit production machinery; the debt begins when an omission violates a stated contract, corrupts state, or makes evidence look stronger than it is.

Use these labels consistently:

- **Repository evidence:** directly visible in tracked code or artifacts.
- **Externally verified fact:** supported by a primary source, chiefly the official ITCH and BinaryFILE specifications.
- **Engineering inference:** a consequence derived from the code and platform model.
- **Unverified or historical claim:** recorded result whose exact build or machine cannot be reconstructed.

The canonical data-structure invariants are in [04](04-data-structures-and-invariants.md), protocol rules in [07](07-itch-protocol-and-replay-semantics.md), and measurement rules in [08](08-benchmarking-and-measurement.md). Solution directions below are alternatives to study, not implementation instructions.

## Correctness and protocol compliance

### HEL-001 — ITCH Price(4) is collapsed to cents — Critical

- **Simple explanation:** Nasdaq can express ten-thousandths of a dollar, but replay throws away the last two decimal places.
- **Technical explanation/evidence:** `BookReplay::priceToCents` divides a raw `Price(4)` by 100 before constructing the book price. Thus 10001 and 10099 both become 100 cents. This is visible in `include/book_replay.hpp`; the decoder itself retains the raw 32-bit field.
- **Violated contract:** distinct legal prices must remain distinct and price ordering must be preserved. The one-to-one mapping between feed price levels and reconstructed levels is lost.
- **Failure example:** orders at $1.0001 and $1.0099 merge into one FIFO queue at $1.00. A delete may still find an ID, but best-price and aggregate-by-price answers no longer describe the feed.
- **Verify:** feed two Add messages for the same side and symbol at raw prices 10001 and 10099; inspect active levels and best price.
- **Why Critical:** it silently corrupts the central product—market-data reconstruction—even on valid input.
- **Directions/tradeoffs:** store raw Price(4) ticks; or use a price type carrying scale. Raw ticks are fastest and exact but every human-facing conversion must respect the scale. A strong type prevents unit mistakes at modest API cost.
- **Prerequisites/interview lesson:** fixed point, units, monotone mappings. Say: “integer prices were correct; my chosen scale was not.”

### HEL-002 — duplicate order IDs can create unreachable live orders — Critical

- **Simple explanation:** adding the same ID twice can leave the first order in a queue while the lookup table points only to the second.
- **Technical explanation/evidence:** `OrderBook::addLimitOrder` links a newly pooled `Order` into a `PriceLevel` and writes `orders_[id]` without first establishing a duplicate policy. Replacing an existing map value does not unlink or release the old order.
- **Violated invariants:** one ID maps to exactly one live order; every queued order is reachable through the ID index; global count matches reachable orders.
- **Failure example:** add ID 7 at bid 100, then ID 7 at bid 101. Cancel 7 removes only the second. Quantity at 100 becomes a ghost.
- **Verify:** run that two-add sequence and compare queue traversal, `orderCount()`, and map reachability.
- **Why Critical:** subsequent valid lifecycle events cannot repair the orphan and aggregate state becomes false.
- **Directions/tradeoffs:** reject duplicates before allocation; treat them as a feed anomaly; or define transactional replacement. Rejecting is clearest for ITCH, where uniqueness among live references is expected.
- **Prerequisites/interview lesson:** referential integrity and commit ordering. An interviewer is testing whether hash-table convenience has hidden a multi-structure transaction.

### HEL-003 — zero-quantity orders can create occupied, non-executable levels — High

- **Simple explanation:** an order with nothing to trade can still make a price look present.
- **Technical explanation/evidence:** the public add path does not reject quantity zero. The queue becomes non-empty, occupancy is set, but a market sweep can consume zero and fail to make progress unless the node is explicitly removed.
- **Violated invariant:** “occupied” should imply a level contains actionable positive quantity, or the system must deliberately define a different contract.
- **Failure example:** add ask `(id=1, price=100, qty=0)` and execute a market buy. Best ask may remain 100 while no quantity can be consumed.
- **Verify:** assert termination, best-price state, aggregate quantity, and count after the sequence.
- **Why High:** it can poison cached best state or create a non-progress loop depending on control flow.
- **Directions/tradeoffs:** reject zero at the API; normalize zero to no-op; or allow it while making all consumers remove zero nodes. Rejecting gives the smallest state space.
- **Prerequisites/interview lesson:** progress invariants. Validate domain constraints at the narrowest trusted boundary.

### HEL-004 — quantity increases retain queue priority — High

- **Simple explanation:** making an order larger can keep an unfairly early place in line.
- **Technical explanation/evidence:** `modifyOrder` changes quantity in place, adjusting the level total but not queue position. In a price-time model, an increase commonly needs a new priority decision; Helios exposes matching-like operations without documenting this semantic.
- **Violated claim:** if Helios claims exchange-style price-time priority for synthetic matching, preserved position on increase is unsupported.
- **Failure example:** A arrives before B at the same price; A increases from 1 to 1,000 and still executes entirely before B.
- **Verify:** add A and B, increase A, sweep, and record execution order.
- **Why High:** it changes synthetic execution results and exposes ambiguity between reconstruction and matching semantics.
- **Directions/tradeoffs:** prohibit increases; move an increased order to the tail; or make modification policy explicit/configurable. Configuration increases surface area and test obligations.
- **Prerequisites/interview lesson:** market rules versus internal mechanics. O(1) modification is not meaningful until semantics are specified.

### HEL-005 — performance build and provenance are inconsistent — High

- **Simple explanation:** the saved number cannot be tied confidently to one compiler, flag set, source revision, or machine.
- **Technical explanation/evidence:** CMake applies optimization in ways that differ by target; library translation units and benchmark translation units need not share the claimed flags. Saved text reports lack a complete manifest.
- **Violated claim:** a performance result should be reproducible and attributable to the binary that produced it.
- **Failure example:** an `-O3 -march=native` benchmark executable calls a separately compiled library built at `-O2` or default optimization, while the report labels the whole workload “O3/native.”
- **Verify:** build verbosely, inspect compile commands for every translation unit, record binary hash and environment, rerun.
- **Why High:** this weakens the project's central “measured engineering” proposition.
- **Directions/tradeoffs:** target-scoped build profiles plus generated manifests; presets improve repeatability but add maintenance.
- **Prerequisites/interview lesson:** translation-unit compilation, linking, reproducible experiments. Flags are properties of compilation actions, not an executable name.

### HEL-006 — the hot path is not generally allocation-free — High

- **Simple explanation:** pooling `Order` objects does not stop the hash table from allocating its own nodes.
- **Technical explanation/evidence:** `ObjectPool<Order>` supplies order storage, while `std::unordered_map<OrderId, Order*>` owns separately allocated nodes and buckets. Rehash and insertion can call the general allocator.
- **Violated claim:** any unconditional “zero allocation add/cancel hot path” statement is broader than repository evidence.
- **Failure example:** reserve too little, then add enough unique orders to trigger rehash; allocator hooks observe calls even though the pool has capacity.
- **Verify:** use a correct allocation counter or allocator instrumentation around warmed operations with controlled capacities.
- **Why High:** allocation and rehash cause latency spikes and invalidate a prominent optimization narrative.
- **Directions/tradeoffs:** reserve adequately; use `pmr`; adopt a flat/open-addressing table; or use a dense ID scheme where possible. Each changes memory use, stability, and worst-case behavior.
- **Prerequisites/interview lesson:** ownership is per container, and “pooled payload” is not “allocation-free graph.”

### HEL-007 — saved replay reports do not match current source behavior — High

- **Simple explanation:** historical output appears to have been produced by a different code version.
- **Technical explanation/evidence:** tracked reports contain labels/counts or behavior not emitted by current replay programs. No commit hash or binary identity connects them.
- **Violated claim:** an artifact should be traceable to its producer.
- **Failure example:** quoting a throughput line during an interview, then discovering the checked-in executable source cannot print that line.
- **Verify:** rebuild current targets and diff output schema; inspect Git history if available.
- **Why High:** it creates credible doubt around every recorded performance conclusion.
- **Directions/tradeoffs:** label artifacts historical; add manifests; regenerate only in a controlled experiment. Retaining history teaches evolution, but it must not masquerade as current evidence.
- **Prerequisites/interview lesson:** experimental provenance and auditability.

### HEL-008 — `alloc_check` exercises invalid prices — High

- **Simple explanation:** the allocation experiment mostly measures rejected operations rather than successful book updates.
- **Technical explanation/evidence:** `tests/alloc_check.cpp` constructs `OrderBook book(100000)`, where that single argument is the minimum price under the current constructor, then submits much lower prices. The range guard rejects them.
- **Violated claim:** a benchmark/check must execute the operation it claims to measure.
- **Failure example:** zero allocations are observed because no order reaches the pool or hash insertion path.
- **Verify:** check return codes and resulting order count; inspect constructor signature.
- **Why High:** it gives false confidence in the allocation claim.
- **Directions/tradeoffs:** use named configuration, assert setup and outcomes, and instrument known-success paths. Strong configuration types prevent positional-argument mistakes.
- **Prerequisites/interview lesson:** benchmark validation. Always prove the workload happened before interpreting counters.

### HEL-009 — an invariant test is tautological — High

- **Simple explanation:** the test calculates a value from the same source twice and calls equality proof.
- **Technical explanation/evidence:** a test in `tests/test_orderbook.cpp` does not independently traverse or model the state it claims to validate; implementation and expected value share the same observation path.
- **Violated claim:** tests should distinguish correct from plausible-but-wrong implementations.
- **Failure example:** corrupt an internal queue while leaving the accessor's cached count unchanged; the assertion still passes.
- **Verify:** mutation-test the implementation or deliberately inject inconsistency and see whether the test fails.
- **Why High:** central structural corruption can survive the suite.
- **Directions/tradeoffs:** add independent traversal/debug validation or a reference model. White-box validators couple to internals; black-box models cost more operations but detect broader faults.
- **Prerequisites/interview lesson:** test oracles and common-mode failure.

### HEL-010 — language-standard documentation disagrees with the build — Medium

- **Simple explanation:** README says C++20 while CMake requests C++17.
- **Technical explanation/evidence:** `README.md` and `CMakeLists.txt` state different contracts. Compiler extensions or a developer default may mask the mismatch.
- **Violated claim:** documented prerequisites must match enforced build requirements.
- **Failure example:** a contributor uses a strict C++17 toolchain and encounters source or expectation differences after following C++20 instructions.
- **Verify:** configure from a clean directory, inspect `compile_commands.json`, build with extensions disabled.
- **Why Medium:** it harms trust and portability more than current runtime correctness.
- **Directions/tradeoffs:** choose the oldest required standard and enforce it target-wise; newer standards enable facilities but narrow toolchain support.
- **Prerequisites/interview lesson:** build system is the executable specification.

### HEL-011 — active tests omit major subsystems — High

- **Simple explanation:** parser, replay adapter, pool behavior, and several test files are not part of the registered automated suite.
- **Technical explanation/evidence:** CMake registers only the principal order-book test target; stub test files and manual programs do not supply automated coverage of protocol offsets or replay lifecycles.
- **Violated claim:** core input and ownership boundaries lack executable verification.
- **Failure example:** swap two ITCH offsets or alter replace sequencing; the suite remains green.
- **Verify:** enumerate `ctest -N`, compare targets against tracked test sources and production headers.
- **Why High:** the most externally constrained logic has no regression net.
- **Directions/tradeoffs:** layered unit, fixture, property, differential, fuzz, and sanitizer tests. More levels cost runtime but catch different fault classes.
- **Prerequisites/interview lesson:** coverage is behavioral, not a count of test files.

### HEL-012 — parser status conflates unsupported and malformed input — High

- **Simple explanation:** callers cannot tell “valid message I do not model” from “broken bytes.”
- **Technical explanation/evidence:** decoder paths return a boolean-like success signal; unknown type, too-short supported frame, and sometimes irrelevant content collapse into limited states.
- **Violated boundary contract:** framing, decoding, support, filtering, and mutation acceptance are distinct classifications.
- **Failure example:** a truncated Add and a valid System Event can increment the same generic skipped counter, hiding corruption.
- **Verify:** construct one example of each category and inspect callback/counter behavior.
- **Why High:** operators and tests cannot reason about feed integrity.
- **Directions/tradeoffs:** return a tagged result with decoded/unsupported/malformed and error detail. Rich results improve observability but add branches and API surface; classify off the hottest mutation path if necessary.
- **Prerequisites/interview lesson:** typed errors and trust boundaries.

### HEL-013 — generic pool destruction skips live non-trivial destructors — High

- **Simple explanation:** if a pooled object owns a resource, destroying the pool may never release that resource through the object's destructor.
- **Technical explanation/evidence:** `ObjectPool<T>` uses raw slot storage and placement construction, but its own destruction relies on block storage teardown rather than tracking and destroying every live `T`. `Order` is currently trivial enough to hide the generic defect.
- **Violated contract:** every successfully constructed non-trivial object must be destroyed exactly once unless the template explicitly restricts `T`.
- **Failure example:** instantiate the pool with a type holding a file descriptor or vector, leave one live, then destroy the pool.
- **Verify:** a destructor-counting type exposes the missing call.
- **Why High:** the template advertises broader correctness than it provides, though current `Order` usage limits immediate damage.
- **Directions/tradeoffs:** constrain `T` to trivially destructible; track live slots; or destroy all live objects on pool teardown. Tracking costs metadata/work.
- **Prerequisites/interview lesson:** raw storage lifetime differs from object lifetime.

### HEL-014 — documentation contains stale claims and placeholder flamegraphs — Medium

- **Simple explanation:** some “evidence” files are dead ends or describe code that has changed.
- **Technical explanation/evidence:** several SVGs contain an error placeholder rather than a profile, and optimization notes describe historical structures or results without version labels.
- **Violated claim:** documentation should identify whether it is current design, historical experiment, or unavailable evidence.
- **Failure example:** a reader follows a flamegraph link expecting stack evidence and finds only “Unable to render.”
- **Verify:** inspect artifact contents and cross-check named symbols against current source.
- **Why Medium:** it does not corrupt execution, but directly harms the project's educational and recruiting credibility.
- **Directions/tradeoffs:** retain with explicit historical banners, replace via reproducible capture, or remove genuinely useless artifacts. Historical failures can be educational when labeled.
- **Prerequisites/interview lesson:** curate evidence; do not curate away uncertainty.

### HEL-015 — 64-byte `Order` alignment has no isolated proof — Medium

- **Simple explanation:** occupying a full cache line sounds fast, but may waste cache capacity and has not been demonstrated to help.
- **Technical explanation/evidence:** `Order` is `alignas(64)` and padded to a cache line. Single-threaded intrusive-node access does not itself require false-sharing isolation; fewer orders then fit per cache and page.
- **Violated claim:** this is not a correctness violation; it is an unsupported optimization claim.
- **Failure example:** random cancellations touch one 64-byte line per order where a compact 32- or 40-byte record could fit more useful nodes in LLC/TLB reach.
- **Verify:** A/B identical workloads with alignment/size variants, report counters and latency distributions.
- **Why Medium:** likely workload-dependent performance cost, not proven regression.
- **Directions/tradeoffs:** compact records, split hot/cold fields, or retain alignment if measurement shows benefit. ABI/layout changes are invasive but reversible internally.
- **Prerequisites/interview lesson:** cache-line alignment prevents some sharing but does not create locality.

### HEL-016 — repository hygiene obscures canonical evidence — Low

- **Simple explanation:** build/output conventions and tracked artifacts make it harder to tell source from generated state.
- **Technical explanation/evidence:** multiple historical result formats, misspelled filenames, and ignore policies do not define a clear artifact lifecycle.
- **Violated claim:** no runtime invariant; the affected property is maintainable provenance.
- **Failure example:** two developers compare differently named reports without knowing which is authoritative.
- **Verify:** inventory tracked outputs and document their producer/version fields.
- **Why Low:** organizational friction rather than direct system failure.
- **Directions/tradeoffs:** a generated-artifact manifest and naming convention; tracking compact evidence helps readers, while large generated data bloats history.
- **Prerequisites/interview lesson:** engineering quality includes the evidence supply chain.

## Boundary safety, replay fidelity, and portability

### HEL-017 — valid prices above the configured ceiling are rejected — High

- **Simple explanation:** replay silently assumes the selected stock never trades above $5,000.
- **Technical explanation/evidence:** the replay book is constructed with a direct-ladder range ending at 500,000 cents. Even before considering HEL-001, a valid feed price outside that configured range cannot be represented.
- **Violated contract:** a protocol decoder/reconstructor should either cover the legal domain or make its narrower admission policy explicit and observable.
- **Failure example:** a valid Add at $5,000.01 passes decoding and symbol filtering but the book rejects it.
- **Verify:** replay a synthetic correctly framed Add immediately below, at, and above the boundary; distinguish accepted mutation from decoded message.
- **Why High:** valid target-symbol data can disappear without a protocol-level reason.
- **Directions/tradeoffs:** configure from observed/reference bounds, use segmented/sparse ladders, or reject with anomaly counters. A full legal-range direct array can be prohibitively large.
- **Prerequisites/interview lesson:** distinguish protocol domain, configured capacity, and current dataset assumptions.

### HEL-018 — invalid constructor ranges can request enormous storage — High

- **Simple explanation:** a bad minimum/maximum pair can wrap an arithmetic calculation and look like a gigantic vector size.
- **Technical explanation/evidence:** ladder length derives from signed/unsigned price-range arithmetic without a complete pre-allocation validation contract. Conversions can turn a negative conceptual size into a huge `size_t`.
- **Violated precondition:** `minPrice <= maxPrice`, representable inclusive span, and an explicit memory budget must hold before allocation.
- **Failure example:** reversed bounds trigger `bad_alloc`, `length_error`, or process pressure instead of a clear configuration error.
- **Verify:** construct with equal, reversed, extreme, and overflowing endpoints under a memory limit.
- **Why High:** untrusted or mistaken configuration can cause availability failure during construction.
- **Directions/tradeoffs:** validate with checked arithmetic before allocating; configuration factories can return typed errors. Exceptions are convenient but should identify the true fault.
- **Prerequisites/interview lesson:** integer conversion and allocation-size validation.

### HEL-019 — `dollarToPrice(double)` truncates and admits floating ambiguity — Medium

- **Simple explanation:** multiplying a binary floating number by 100 and casting can turn a human price into the wrong cent.
- **Technical explanation/evidence:** the convenience conversion uses `double` and truncation rather than an exact decimal parser or stated rounding rule.
- **Violated contract:** external decimal-to-fixed-point conversion needs an explicit scale, admissible precision, and rounding policy.
- **Failure example:** a decimal represented just below an integer-cent boundary becomes one cent lower after truncation.
- **Verify:** inspect boundary values with `nextafter`, negative inputs, and more than two decimal places.
- **Why Medium:** core integer APIs can avoid it, but the helper invites subtle wrong inputs.
- **Directions/tradeoffs:** accept integer ticks, parse decimal strings, or use checked rounding. String parsing is exact but slower and belongs off the hot path.
- **Prerequisites/interview lesson:** fixed-point storage does not help if conversion is lossy.

### HEL-020 — unsigned timestamp differences are narrowed to signed — Medium

- **Simple explanation:** a large cycle count difference can be displayed as a negative or implementation-dependent value.
- **Technical explanation/evidence:** timer samples originate as `uint64_t`, while subtraction or reporting paths use `int64_t`/signed arithmetic. Wraparound and narrowing rules are not guarded.
- **Violated contract:** elapsed-cycle arithmetic must define counter wrap and preserve its representable range.
- **Failure example:** a synthetic near-wrap pair or an extremely long interval produces a nonsensical signed latency.
- **Verify:** unit-test arithmetic with values near `UINT64_MAX` and `INT64_MAX` independently of the hardware clock.
- **Why Medium:** ordinary short measurements are unaffected, but the utility's generic correctness is weaker than its interface suggests.
- **Directions/tradeoffs:** keep modular unsigned differences and validate interval bounds; use a result type that can report invalid samples.
- **Prerequisites/interview lesson:** timing correctness starts with integer semantics.

### HEL-021 — add is not transactional under allocation failure — High

- **Simple explanation:** the book may change one structure and then fail while changing another.
- **Technical explanation/evidence:** an order can be acquired/constructed and linked or prepared before `unordered_map` insertion, whose node allocation may throw. The path has no explicit rollback guard spanning pool, level, bitmap, counters, and map.
- **Violated invariant:** every public mutation must either commit all correlated structures or leave the prior state intact.
- **Failure example:** inject `bad_alloc` at hash insertion and inspect pool live state, queue membership, aggregate quantity, and count.
- **Verify:** use a throwing allocator or fault injection at each commit step.
- **Why High:** memory pressure can convert a recoverable failure into permanent internal inconsistency.
- **Directions/tradeoffs:** reserve first, insert map transactionally, use scope guards, or declare fail-fast/no-exception process policy. Production low-latency systems often preallocate, but the policy must be explicit.
- **Prerequisites/interview lesson:** exception safety levels and multi-index commit protocols.

### HEL-022 — decoders check minimums, not exact/domain validity — High

- **Simple explanation:** enough bytes are treated as sufficient even when a message is too long or its fields are nonsensical.
- **Technical explanation/evidence:** decoder cases primarily protect offset reads with minimum-length checks. They do not consistently enforce the specification's exact payload length or validate side/domain fields.
- **Violated protocol contract:** a framed type has an expected length and field domain; accepting arbitrary extensions hides framing/data errors.
- **Failure example:** an Add payload with extra bytes or invalid buy/sell indicator can enter the decoded path.
- **Verify:** mutate each supported fixture's length and enumerated fields; require distinct malformed results.
- **Why High:** malformed feeds can be accepted and contaminate state or anomaly accounting.
- **Directions/tradeoffs:** strict validation by default, perhaps a separately named permissive mode for research captures. Validation branches cost little relative to correctness at ingress.
- **Prerequisites/interview lesson:** bounds safety is necessary but not protocol compliance.

### HEL-023 — replay throughput denominators mix different populations — Medium

- **Simple explanation:** “messages per second” may divide by decoded messages even though the program scanned many more frames.
- **Technical explanation/evidence:** counters distinguish some supported/processed messages, while elapsed replay time includes framing, unsupported types, filtering, and rejected mutations. Report labels do not always state which count is the denominator.
- **Violated measurement claim:** numerator and measured boundary must describe the same workload population.
- **Failure example:** two files with equal supported counts but very different unsupported traffic report incomparable rates.
- **Verify:** report bytes, frames, supported decoded, target-symbol, attempted mutations, and accepted mutations simultaneously.
- **Why Medium:** results can be interpreted incorrectly but raw replay still functions.
- **Directions/tradeoffs:** publish a rate vector instead of one headline number; more counters improve meaning with negligible out-of-timed reporting cost.
- **Prerequisites/interview lesson:** measurement semantics are part of the API.

### HEL-024 — symbol filtering copies raw bytes and ignores stronger identity — Medium

- **Simple explanation:** the adapter repeatedly compares an eight-byte name even though ITCH supplies a compact stock-locate identifier.
- **Technical explanation/evidence:** decoded Add messages carry the fixed-width stock field; replay copies/compares it against the target. `stockLocate` is decoded but not used to establish a directory-backed identity.
- **Violated claim:** no correctness failure for well-formed single-symbol data, but the filter discards protocol structure and adds hot-path work.
- **Failure example:** malformed or inconsistent stock text/locate combinations cannot be detected; every Add performs an eight-byte comparison.
- **Verify:** profile comparison share and construct inconsistent synthetic metadata.
- **Why Medium:** modest performance and semantic-observability issue, important for multi-symbol evolution.
- **Directions/tradeoffs:** learn symbol-to-locate mapping from directory messages, then filter by integer; retain raw symbol validation. Directory state increases lifecycle complexity.
- **Prerequisites/interview lesson:** identifiers require establishment and validation, not blind substitution.

### HEL-025 — replay script command construction and exit handling are weak — Medium

- **Simple explanation:** the script can report a run without proving the command was safe or successful.
- **Technical explanation/evidence:** `run_itch_replays.sh` assembles paths/commands and orchestrates downloads/runs with limited validation of tools, quoting, checksums, and child exit states.
- **Violated operational contract:** automation should fail closed and preserve which input/binary produced an output.
- **Failure example:** a failed download leaves a partial file that is later replayed; or a failed executable still yields a report wrapper.
- **Verify:** run with missing tool, bad URL, space-containing path, failed checksum, and nonzero replay status.
- **Why Medium:** it can produce misleading artifacts or unsafe assumptions, not corrupt the core book directly.
- **Directions/tradeoffs:** strict shell mode, explicit arrays/quoting, checksums, manifests, and atomic downloads. Portability across shell environments requires care.
- **Prerequisites/interview lesson:** benchmark automation is production code for evidence.

### HEL-026 — memory advice and prefault operations are unchecked — Medium

- **Simple explanation:** the replay claims memory preparation without verifying that the operating system accepted it.
- **Technical explanation/evidence:** mapped-file preparation assumes a page size/access policy and does not consistently check `madvise` or related outcomes. Touching pages also changes what the timed run includes.
- **Violated measurement claim:** OS-state preparation must be checked and documented.
- **Failure example:** advice fails, pages remain cold, yet the result is labeled prefaulted; a hard-coded page stride misses the actual environment's page granularity.
- **Verify:** check return values/`errno`, query page size, and report major/minor faults for cold and warm modes.
- **Why Medium:** mainly undermines repeatability and interpretation.
- **Directions/tradeoffs:** explicit cold-cache and warm-cache experiments; checked advice; configurable prefault. Neither mode is universally “correct”—they answer different questions.
- **Prerequisites/interview lesson:** `mmap` establishes a mapping, not residency.

### HEL-027 — best-price refresh may scan bitmap words — Medium

- **Simple explanation:** finding the next best price is fast relative to scanning every price, but is not always constant time.
- **Technical explanation/evidence:** when the best level empties, refresh searches occupancy bitmap words until finding a set bit. Cost depends on distance to the next occupied word/range width.
- **Violated claim:** stale documentation suggests stronger O(1) behavior than the implementation guarantees.
- **Failure example:** clear the only best bid with the next occupied price far away; refresh touches many zero words.
- **Verify:** benchmark removal with controlled gaps and inspect latency against gap in bitmap words.
- **Why Medium:** tail latency and complexity claims, not correctness.
- **Directions/tradeoffs:** hierarchical bitmaps, summary blocks, trees, or accept bounded scan for chosen range. More hierarchy adds update work and memory.
- **Prerequisites/interview lesson:** distinguish expected, amortized, bounded-domain, and worst-case complexity.

### HEL-028 — the primary workload is add-only and grows state — High

- **Simple explanation:** timing only adds into an ever-growing book does not represent a steady order lifecycle.
- **Technical explanation/evidence:** the principal benchmark emphasizes adds, so hash load, pool growth, price occupancy, and working-set size change over the run. Cancels, executes, replacements, and mixed locality are absent.
- **Violated measurement claim:** a result cannot be generalized to “order-book latency” without naming the add-only distribution and state trajectory.
- **Failure example:** early samples hit cache-resident structures; late samples incur rehash/growth/cache pressure; one aggregate hides both.
- **Verify:** bucket results by operation index/state size and compare steady-state mixed traces.
- **Why High:** it supports an overly broad headline about the project's central performance.
- **Directions/tradeoffs:** separate microbenchmarks from trace/mixed-state benchmarks; controlled synthetic distributions aid causality while replay aids realism.
- **Prerequisites/interview lesson:** workload is part of the result.

### HEL-029 — timer-overhead subtraction and “worst case” statistic are invalidly strong — High

- **Simple explanation:** subtracting the smallest timer reading does not reveal the operation's true time, and the median of several maxima is not the worst observed latency.
- **Technical explanation/evidence:** timer and operation costs can overlap/non-add; minimum overhead is a biased estimator. Aggregating each run's maximum with a median deliberately discards higher maxima.
- **Violated measurement claim:** reported labels overstate what the statistic estimates.
- **Failure example:** one run contains a genuine 10× outlier; median-of-maxima excludes it while output says “worst case.”
- **Verify:** retain raw samples, report unadjusted boundaries, distributions, run maxima, global maximum, and a stated estimator.
- **Why High:** misleading tail claims are especially damaging in low-latency work.
- **Directions/tradeoffs:** batch timing amortizes timer cost but loses per-op tails; paired calibration can characterize instrumentation but not magically subtract it. Report both when useful.
- **Prerequisites/interview lesson:** measurement is a model with assumptions, not arithmetic cleanup.

### HEL-030 — CPU affinity failures are ignored — High

- **Simple explanation:** the benchmark can say it pinned itself even when the operating system refused.
- **Technical explanation/evidence:** affinity setup is attempted without a hard checked precondition and environment report. Core migration can change TSC auxiliary data, cache warmth, and scheduling noise.
- **Violated measurement claim:** “pinned-core latency” requires verified placement.
- **Failure example:** insufficient permissions or an invalid CPU ID returns failure; measurement proceeds and is labeled pinned.
- **Verify:** check the API result, read back affinity, record CPU before/after samples.
- **Why High:** it silently changes the experiment.
- **Directions/tradeoffs:** fail the benchmark when pinning is required, or label results unpinned; make CPU selection explicit.
- **Prerequisites/interview lesson:** every environmental control needs an assertion.

### HEL-031 — timer capability and migration metadata are not validated — High

- **Simple explanation:** the code reads a hardware counter without proving the platform provides the ordering and stability the analysis assumes.
- **Technical explanation/evidence:** x86 paths use `RDTSC/RDTSCP`, but capability/invariant-TSC checks and `RDTSCP` auxiliary migration checks are absent. Serialization semantics depend on the chosen fence sequence and architecture.
- **Violated measurement claim:** cycle deltas are meaningful only under stated counter, ordering, and placement conditions.
- **Failure example:** migration occurs between start/end or unsupported semantics differ; the sample remains in the distribution.
- **Verify:** CPUID capability flags, OS clocksource/environment, auxiliary core IDs, and comparison with a monotonic clock.
- **Why High:** foundational timer uncertainty contaminates all fine-grained numbers.
- **Directions/tradeoffs:** validated TSC harness on supported x86; portable monotonic clock fallback; discard migrated samples. Portability often reduces resolution.
- **Prerequisites/interview lesson:** a timestamp instruction is not by itself a benchmark methodology.

### HEL-032 — profiling includes random generation and bookkeeping — Medium

- **Simple explanation:** the profiler spends time choosing operations and recording them, so its hot functions are not purely the book's hot functions.
- **Technical explanation/evidence:** `profile_driver.cpp` performs RNG, distributions, and driver-side state management in the profiled process/region.
- **Violated claim:** a whole-program profile cannot be presented as a pure data-structure profile unless the driver share is separated.
- **Failure example:** RNG arithmetic dominates samples, making an order-book optimization invisible.
- **Verify:** pre-generate a trace, profile driver-only and book-only variants, compare call stacks.
- **Why Medium:** profiles remain useful for end-to-end workload cost, but attribution can be wrong.
- **Directions/tradeoffs:** pre-generated deterministic traces isolate the subject but alter cache traffic; retain an end-to-end version too.
- **Prerequisites/interview lesson:** the observer and workload generator are part of the measured program.

## Evidence, testing, and architectural limits

### HEL-033 — tail-latency claims exceed the sample evidence — High

- **Simple explanation:** a percentile or maximum from a short, biased run is not a universal tail guarantee.
- **Technical explanation/evidence:** reports contain nanosecond/cycle summaries without raw-sample archives, confidence analysis, long-run stationarity checks, or OS/interrupt context.
- **Violated claim:** production-style tail statements require enough observations, a defined population, and controlled censoring/outliers.
- **Failure example:** a million-operation run never encounters pool growth or a scheduler interrupt that occurs every few seconds in deployment.
- **Verify:** preserve raw samples/run metadata; repeat across time; stratify fast and slow paths; report sample count and confidence limits where appropriate.
- **Why High:** low-latency recruiting claims are judged most harshly at the tail.
- **Directions/tradeoffs:** cautious labels (“observed p99.9 in this run”) and long controlled runs; raw data costs storage but enables reanalysis.
- **Prerequisites/interview lesson:** an observed maximum is not a worst-case bound.

### HEL-034 — tests retain IDs after synthetic execution removes them — Medium

- **Simple explanation:** the test driver's idea of live orders can become stale after market sweeps.
- **Technical explanation/evidence:** manual/randomized test bookkeeping tracks IDs separately but does not fully reconcile fills performed inside the book. Later operations may target already-dead IDs and interpret expected rejection as workload behavior.
- **Violated test invariant:** the generator's model of live IDs must match the system or intentionally include a known invalid-operation distribution.
- **Failure example:** full execution removes ID 5; the driver later chooses 5 for cancel and never checks whether this was deliberately invalid.
- **Verify:** compare driver live set to an independent book snapshot after every operation.
- **Why Medium:** weakens randomized/manual evidence rather than core operation correctness.
- **Directions/tradeoffs:** use a reference model that returns executions; generate from model state; separately generate invalid cases with labels.
- **Prerequisites/interview lesson:** a test harness also has state and invariants.

### HEL-035 — pool growth and rehash remain latency cliffs — High

- **Simple explanation:** fast steady operations occasionally pay for allocating a new block or rebuilding the hash table.
- **Technical explanation/evidence:** pool capacity grows in blocks and `unordered_map` grows by allocating/rebucketing. Neither slow path is excluded by an enforced operating-capacity contract.
- **Violated performance claim:** “bounded/consistent hot path” is false unless capacity is pre-sized and monitored.
- **Failure example:** the next add crosses both a pool-block boundary and hash load threshold, producing a large outlier.
- **Verify:** time operations around known capacity thresholds and instrument allocator/rehash events.
- **Why High:** deterministic latency is a central project goal.
- **Directions/tradeoffs:** pre-size with admission limits; grow only during controlled phases; or accept elasticity and expose slow-path counters. Capacity bounds trade flexibility for predictability.
- **Prerequisites/interview lesson:** amortized O(1) explicitly permits expensive individual operations.

### HEL-036 — replay eagerly provisions a very large direct book — Medium

- **Simple explanation:** one-symbol replay allocates ladders for the whole configured range before learning what prices occur.
- **Technical explanation/evidence:** two vectors of `PriceLevel` plus occupancy maps are sized from the fixed 1..500000-cent range. `PriceLevel` vectors construct every element even if only a narrow price band is used.
- **Violated scalability goal:** memory is proportional to configured range per book, not active prices.
- **Failure example:** multiplying the design across thousands of symbols consumes prohibitive virtual/resident memory and construction time.
- **Verify:** calculate `2 * levels * sizeof(PriceLevel)` plus bitmap/vector overhead and measure construction/RSS.
- **Why Medium:** acceptable for the present one-symbol experiment, limiting for evolution.
- **Directions/tradeoffs:** per-symbol ranges, segmented ladders, sparse maps, or hybrid hot bands. Direct arrays win when range density and memory budget cooperate.
- **Prerequisites/interview lesson:** O(1) indexing buys time with address space, initialization, and cache footprint.

### HEL-037 — replay lacks session sequencing and recovery semantics — High

- **Simple explanation:** Helios applies messages in file order but cannot prove the feed is complete or recover from missing data.
- **Technical explanation/evidence:** framing/decoding does not model transport sequence numbers, redundant channels, gaps, snapshots, session state, or restart boundaries.
- **Violated production assumption:** deterministic application of bytes is not equivalent to a complete, trustworthy live feed.
- **Failure example:** remove one Delete frame from a capture; replay finishes with a plausible but wrong book and no gap alarm.
- **Verify:** mutate a fixture by dropping/reordering frames and observe the lack of sequence-level detection.
- **Why High:** a major limitation if framed as production infrastructure; legitimate scope if clearly documented as offline reconstruction.
- **Directions/tradeoffs:** preserve capture/session sequence metadata and build validation/recovery around the decoder. This is an architectural expansion, not a local fix.
- **Prerequisites/interview lesson:** protocol payload correctness and transport/session completeness are different layers.

### HEL-038 — timestamp and feed metadata are discarded before mutation — Medium

- **Simple explanation:** the book knows the final state but cannot explain when or through which feed metadata an event arrived.
- **Technical explanation/evidence:** decoded stock locate, tracking number, timestamp, match numbers, and attribution are mostly not retained by `OrderBook`; the adapter reduces events to local operations.
- **Violated capability:** no current invariant requires retention, but audit, deterministic state hashing by event, latency analysis, and anomaly diagnosis need provenance.
- **Failure example:** two inconsistent events produce a wrong state, but the final book cannot identify the causative feed timestamp/reference.
- **Verify:** inspect adapter call signatures and state; attempt to emit a forensic event trail.
- **Why Medium:** expected in a minimal book, constraining for debugging/production evolution.
- **Directions/tradeoffs:** structured event journal/counters outside the hot book; retaining everything increases bandwidth and storage.
- **Prerequisites/interview lesson:** separate minimal mutation state from observability state.

### HEL-039 — `printBook` is a public no-op — Low

- **Simple explanation:** an advertised debugging function produces no useful book view.
- **Technical explanation/evidence:** `OrderBook::printBook` exists but its implementation is empty/stub-like.
- **Violated API contract:** callers infer a visible diagnostic effect from the name.
- **Failure example:** a test or debugging session calls it after corruption and receives no evidence.
- **Verify:** invoke it on a non-empty book.
- **Why Low:** no hot-path correctness impact; it increases debugging friction and API noise.
- **Directions/tradeoffs:** implement an explicitly cold snapshot/formatter, or remove/deprecate the API. Formatting should not contaminate hot structures.
- **Prerequisites/interview lesson:** unfinished public APIs are liabilities even when harmless.

### HEL-040 — non-x86 timer fallback silently returns zero — High

- **Simple explanation:** on another architecture the benchmark can “succeed” with impossible zero-cycle measurements.
- **Technical explanation/evidence:** `rdtsc_timer.hpp` provides an architecture conditional whose unsupported path returns zero rather than refusing or using a meaningful clock.
- **Violated measurement contract:** unsupported measurement must be explicit; zero is a valid-looking data point.
- **Failure example:** build on ARM, obtain all-zero deltas or division artifacts, and record them as results.
- **Verify:** compile/test the fallback path or inspect preprocessor output.
- **Why High:** silent fabrication of measurement data is worse than a clear portability limit.
- **Directions/tradeoffs:** compile-time error for cycle mode; architecture-specific counter with validation; or `steady_clock` fallback with different labels.
- **Prerequisites/interview lesson:** portable compilation is not portable semantics.

### HEL-041 — `activeBids`/`activeAsks` count levels, not orders — Low

- **Simple explanation:** the names sound like order counts, but the values are occupied price-level counts.
- **Technical explanation/evidence:** book accessors derive these values from level occupancy, while a separate global count represents orders.
- **Violated API property:** names should reveal the unit and aggregation.
- **Failure example:** one level containing 10,000 bids reports `activeBids == 1`; a caller sizes work incorrectly.
- **Verify:** add multiple same-price orders and inspect the accessor.
- **Why Low:** behavior can be learned and is internally coherent, but invites misuse.
- **Directions/tradeoffs:** rename to `activeBidLevels`/`activeAskLevels`, or document units. Compatibility is the only cost.
- **Prerequisites/interview lesson:** units belong in names and types.

### HEL-042 — reconstruction and matching semantics are blurred — High

- **Simple explanation:** the project both reconstructs a displayed feed and invents market-order execution, which are different products.
- **Technical explanation/evidence:** ITCH replay applies exchange-reported lifecycle events, while `executeMarketOrder` locally chooses resting orders. Modification semantics further imply a matching policy not supplied by ITCH.
- **Violated conceptual boundary:** a market-data book reports observed exchange state; a matching engine decides outcomes and must implement venue rules.
- **Failure example:** a synthetic sweep is described as what Nasdaq “would execute,” despite missing hidden orders, routing, auction, and venue rule context.
- **Verify:** classify every public mutation as observed-event application or locally decided matching.
- **Why High:** conceptual overclaim is a frequent HFT interview failure.
- **Directions/tradeoffs:** separate reconstruction core and synthetic simulator interfaces; state that simulation is an educational consumer, not venue emulation.
- **Prerequisites/interview lesson:** defend scope precisely: Helios is not a matching engine.

### HEL-043 — optimization comparisons lack controlled provenance — Medium

- **Simple explanation:** before/after files cannot prove the code change caused the difference.
- **Technical explanation/evidence:** alignment, array, pool, and other result files omit one or more of source revision, flags, hardware, kernel state, run protocol, or raw samples.
- **Violated experimental claim:** causal performance comparison requires controlled variables and attributable binaries.
- **Failure example:** an “aligned improvement” was actually captured under a different compiler or power state.
- **Verify:** attempt to reconstruct each table solely from tracked source and metadata; unresolved degrees of freedom remain.
- **Why Medium:** historical learning may be real, but evidence is insufficient for a strong public claim.
- **Directions/tradeoffs:** experiment manifests, repeatable scripts, randomized A/B order, confidence intervals, and archived raw data.
- **Prerequisites/interview lesson:** optimization diary entries are hypotheses until reproducibly tested.

### HEL-044 — pool page-fault explanation conflicts with eager slot writes — Medium

- **Simple explanation:** documentation says pool pages stay untouched, but building the free list writes every slot.
- **Technical explanation/evidence:** block creation initializes intrusive free-list links across slots, which touches memory. Notes attributing later behavior to completely lazy untouched pages do not match this implementation.
- **Violated claim:** the explanation of measured page-fault/locality behavior is inconsistent with repository evidence.
- **Failure example:** an interviewer traces `addBlock` and asks how pages can be untouched after each slot's pointer was stored.
- **Verify:** inspect writes and measure minor faults/residency immediately after construction versus first allocation.
- **Why Medium:** conceptual/profiling interpretation issue, not a functional defect.
- **Directions/tradeoffs:** correct the narrative; alternatively design a lazy free-list scheme, which trades first-use work for construction cost.
- **Prerequisites/interview lesson:** explain the code that exists, then measure OS effects; do not backfill a convenient story.

### HEL-045 — custom hash-map experiment cannot be reproduced — Low

- **Simple explanation:** notes discuss an alternative implementation that is not present with runnable evidence.
- **Technical explanation/evidence:** optimization documentation mentions a custom hash-map attempt and conclusions, but the source/configuration/workload needed to repeat it is absent.
- **Violated evidence property:** historical design learning lacks a reproducible artifact.
- **Failure example:** no reviewer can determine whether failure came from load factor, hash quality, probing, tombstones, or workload.
- **Verify:** search tracked files for the implementation and exact benchmark recipe.
- **Why Low:** no current runtime fault; missed educational value.
- **Directions/tradeoffs:** preserve experiments in branches/tags or a compact journal with patch, manifest, and result. Avoid keeping dead alternatives in production paths.
- **Prerequisites/interview lesson:** reversed optimizations are powerful stories only with mechanism and evidence.

### HEL-046 — no CI or compiler/platform matrix — Medium

- **Simple explanation:** nothing automatically proves that a clean checkout builds and tests across supported environments.
- **Technical explanation/evidence:** no tracked CI workflow or declared support matrix compiles strict variants, runs tests, or exercises sanitizer configurations.
- **Violated maintenance goal:** compatibility claims need recurring verification.
- **Failure example:** a header relies on a transitive include that works with one standard library but fails with another.
- **Verify:** perform clean GCC/Clang, Debug/Release, sanitizer, and supported-OS builds.
- **Why Medium:** defect detection and recruiting polish, not current functional proof.
- **Directions/tradeoffs:** small matrix on every change, extended scheduled jobs. More configurations increase feedback time and maintenance.
- **Prerequisites/interview lesson:** reproducibility begins with clean automation, not a developer's build directory.

### HEL-047 — global build policy and mandatory GTest reduce composability — Medium

- **Simple explanation:** consumers inherit flags they did not ask for, and configuring the project requires test machinery even when they only want the library.
- **Technical explanation/evidence:** CMake mixes global and target concerns and resolves GoogleTest without a clear optional test boundary.
- **Violated build-design principle:** usage requirements and developer-only policy should be scoped to the targets/options that need them.
- **Failure example:** embedding Helios in another build changes warnings/optimization unexpectedly or fails configuration because GTest is unavailable.
- **Verify:** add Helios with `add_subdirectory` in a minimal parent project and configure with tests undesired.
- **Why Medium:** limits reuse, packaging, and clean experimental profiles.
- **Directions/tradeoffs:** target-scoped options, `BUILD_TESTING`, presets/toolchains, explicit benchmark option. More options require documentation but clarify the graph.
- **Prerequisites/interview lesson:** modern CMake models a dependency graph, not a global compiler command.

### HEL-048 — saved outputs mix implementations and environments without a manifest — Medium

- **Simple explanation:** the result directory is a scrapbook, not a queryable experiment record.
- **Technical explanation/evidence:** filenames and report text span baseline, pooled, aligned, replay, and profiling states without a uniform schema for revision, dirty tree, compiler, flags, CPU, OS, input checksum, and command.
- **Violated evidence property:** comparisons need identity and lineage.
- **Failure example:** two “million ops” reports differ, but the reader cannot isolate implementation change from hardware/build change.
- **Verify:** build a table of required manifest fields for each tracked artifact; observe missing cells.
- **Why Medium:** cumulative credibility cost across all performance claims.
- **Directions/tradeoffs:** immutable experiment directories with machine-readable manifest, raw data, summary, and analysis; or keep only representative documented runs. Large binary traces can live outside Git with checksums.
- **Prerequisites/interview lesson:** performance results are data products.

## Severity synthesis

| Severity | Findings | Educational interpretation |
|---|---|---|
| Critical | HEL-001, HEL-002 | The reconstructed state can be silently wrong on valid or plausible input. |
| High | HEL-003–009, 011–013, 017–018, 021–022, 028–031, 033, 035, 037, 040, 042 | Correctness boundaries, test trust, or core latency claims can fail materially. |
| Medium | HEL-010, 014–015, 019–020, 023–027, 032, 034, 036, 038, 043–044, 046–048 | Important maintainability, fidelity, scalability, and evidence limitations. |
| Low | HEL-016, 039, 041, 045 | Cleanup/API/history issues with low immediate runtime risk. |

Severity does not determine learning order. Begin with units and invariants (HEL-001/002/003), then build an independent oracle (HEL-009/011/012/022), then repair the experiment model conceptually (HEL-005/006/028–033). That sequence teaches specification, state-machine reasoning, and measurement discipline before optimization.

## Cross-cutting lessons

1. **A book mutation is a transaction across representations.** Queue, aggregate, bitmap, cache, ID index, counter, and pool state must commit together.
2. **Ingress needs a taxonomy.** Framed, supported, decoded, target-symbol, valid-lifecycle, accepted, and rejected are not synonyms.
3. **An optimization claim needs a counterfactual.** Without comparable builds and raw observations, a mechanism remains a hypothesis.
4. **Scope is a correctness property.** Calling reconstruction “matching” or an observed tail “worst case” changes what a reader believes was proved.
5. **The best defense is calibrated honesty.** State what is implemented, what is measured, what is inferred, and what remains unknown.

Continue with the hypothetical production boundary in [13](13-production-redesign.md) and practice defending these limitations in [14](14-interview-defense-guide.md).
