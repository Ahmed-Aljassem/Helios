# Helios Glossary

Each entry has four parts: **Plain** meaning; **Precise** model; **Helios** location/use; **Related** terms. Terms are grouped for study, not alphabetically across the whole chapter.

## Market and order-book terms

### Ask / offer

**Plain:** an intention to sell. **Precise:** a resting sell order with a limit price and remaining quantity. **Helios:** ask ladder and synthetic market-buy consumption. **Related:** bid, spread, best ask.

### Bid

**Plain:** an intention to buy. **Precise:** a resting buy order with a maximum acceptable limit price. **Helios:** bid ladder and synthetic market-sell consumption. **Related:** ask, best bid, side.

### Best bid / best ask

**Plain:** highest offered buying price / lowest offered selling price. **Precise:** extrema among occupied bid/ask price levels. **Helios:** cached ladder indexes backed by occupancy bitmaps. **Related:** top of book, spread, NBBO.

### Displayed book

**Plain:** the orders the market data feed exposes. **Precise:** a reconstruction of disseminated visible interest, not necessarily all venue liquidity. **Helios:** the intended meaning of ITCH replay state. **Related:** hidden liquidity, reconstruction.

### Execution / fill

**Plain:** some or all of an order traded. **Precise:** a lifecycle event reducing remaining quantity, optionally removing the order at zero. **Helios:** ITCH E/C adapter paths and synthetic sweeps. **Related:** partial execution, full execution, match number.

### Limit order

**Plain:** buy no higher than, or sell no lower than, a chosen price. **Precise:** side, limit price, quantity, identity, and priority-bearing arrival. **Helios:** `Order` stored at exactly one `PriceLevel`. **Related:** market order, price-time priority.

### Market order

**Plain:** trade now against available prices. **Precise:** an aggressive instruction without a resting limit in the simplified model; actual venues apply richer rules. **Helios:** synthetic `executeMarketOrder`, not an ITCH inbound instruction. **Related:** matching engine, sweep.

### Matching engine

**Plain:** the authority that decides which orders trade. **Precise:** venue component accepting order entry and applying a rulebook to create authoritative executions. **Helios:** not implemented; synthetic execution is educational simulation. **Related:** reconstruction, price-time priority.

### Order book

**Plain:** organized record of outstanding buys and sells. **Precise:** state indexed by order identity and ordered by side, price, and time. **Helios:** `OrderBook` combines ID map, ladders, levels, bitmaps, caches, and pool. **Related:** price level, queue.

### Price level

**Plain:** all orders at one price. **Precise:** a FIFO sequence plus aggregate remaining quantity and order count for a side/price pair. **Helios:** `PriceLevel`. **Related:** ladder, price-time priority.

### Price-time priority / FIFO

**Plain:** better prices go first; equal prices keep arrival order. **Precise:** lexicographic priority by side-adjusted price then sequence/arrival time. **Helios:** best-price selection plus intrusive head-to-tail queues. **Related:** modification priority, queue.

### Spread

**Plain:** gap between best sell and best buy. **Precise:** `bestAsk - bestBid` when both sides exist. **Helios:** derivable from cached best indexes; not an exchange-wide NBBO. **Related:** crossed book, locked book.

### Reconstruction

**Plain:** rebuild state from reported events. **Precise:** deterministic application of market-data lifecycle messages, assuming ordered/complete input. **Helios:** `BookReplay`. **Related:** replay, matching engine.

### Side

**Plain:** buy or sell. **Precise:** an enumerated order attribute controlling ladder, price preference, and execution direction. **Helios:** `Side`, decoded from ITCH buy/sell indicator. **Related:** bid, ask.

## ITCH and ingestion terms

### BinaryFILE

**Plain:** a file containing length-prefixed messages. **Precise:** Nasdaq framing with a two-byte big-endian message length followed by that many payload bytes. **Helios:** `ITCHParser::parseBuffer`. **Related:** frame, payload, endianness.

### Big-endian / network byte order

**Plain:** most significant byte comes first. **Precise:** byte serialization order independent of the host's integer representation. **Helios:** explicit shift/or decoder helpers. **Related:** little-endian, wire format.

### Decoder

**Plain:** turns bytes into named fields. **Precise:** validates sufficient/exact input and constructs a typed representation according to offsets and field widths. **Helios:** type-specific paths in `itch_parser.hpp`. **Related:** parser, framing.

### Frame

**Plain:** one message boundary. **Precise:** length prefix plus bounded payload region in BinaryFILE. **Helios:** outer parse loop. **Related:** truncation, payload.

### Message type

**Plain:** one-character label saying what event follows. **Precise:** first ITCH payload byte selecting a schema such as A, F, E, C, X, D, or U. **Helios:** decoder dispatch. **Related:** protocol, schema.

### MPID / attribution

**Plain:** identifier for a market participant when attribution is displayed. **Precise:** four-byte attribution field on Add Order with MPID Attribution. **Helios:** decoded F message metadata, not material to book ordering. **Related:** Add Order.

### Order reference number

**Plain:** feed identity of an order. **Precise:** 64-bit reference used by later lifecycle messages; replacement introduces a new reference. **Helios:** `OrderId` and ID index. **Related:** duplicate ID, replace.

### Price(4)

**Plain:** dollar price with four implied decimal places. **Precise:** unsigned integer in units of $0.0001. **Helios:** raw decoder field; incorrectly divided to cents by current replay. **Related:** fixed point, tick.

### Stock locate

**Plain:** compact feed identifier for a security. **Precise:** 16-bit value assigned in ITCH directory/session context. **Helios:** decoded but not used for the one-symbol filter. **Related:** symbol directory.

### Symbol field

**Plain:** fixed-width stock name. **Precise:** eight-byte, space-padded stock field on relevant ITCH messages. **Helios:** raw comparison in `BookReplay`. **Related:** stock locate.

### Timestamp

**Plain:** when the event occurred in the feed's session. **Precise:** six-byte nanoseconds-since-midnight field in ITCH message headers. **Helios:** decoded metadata largely discarded before book mutation. **Related:** tracking number, replay order.

### Tracking number

**Plain:** feed tracking metadata. **Precise:** common header field distinct from order reference and transport sequence. **Helios:** decoded but not used for state. **Related:** sequencing, timestamp.

### Session termination

**Plain:** signal/end condition that a session or stream is finished. **Precise:** feed/session lifecycle concept distinct from reaching EOF in a capture. **Helios:** complete session-state handling is not modeled. **Related:** system event, recovery.

### Sequencing / gap

**Plain:** messages have an order; a gap means one is missing. **Precise:** transport/session sequence validation detects discontinuity and triggers recovery policy. **Helios:** not implemented for the raw BinaryFILE replay abstraction. **Related:** redundant feed, snapshot.

## Data structures and C++ terms

### Aggregate quantity

**Plain:** total remaining shares at a price. **Precise:** sum of `quantity` over all reachable orders in a level. **Helios:** cached in `PriceLevel::totalQuantity`. **Related:** derived state, invariant.

### Cached value / derived state

**Plain:** an answer stored so it need not be recomputed. **Precise:** redundant state whose update protocol must keep it equivalent to canonical data. **Helios:** best indexes, aggregate quantities, counts, occupancy. **Related:** invariant.

### Direct-mapped ladder

**Plain:** price directly selects an array slot. **Precise:** `index = price - minimum`, giving constant arithmetic lookup within a fixed inclusive domain. **Helios:** bid/ask vectors. **Related:** sparse tree, price level.

### Occupancy bitmap

**Plain:** compact yes/no markers for non-empty levels. **Precise:** bit vector where bit `i` is set iff ladder level `i` is occupied. **Helios:** next-best discovery and active-level tracking. **Related:** bit scan, summary bitmap.

### Bit scan

**Plain:** find the next set marker quickly. **Precise:** word search plus count-leading/trailing-zero or equivalent operation. **Helios:** best-price refresh. **Related:** bitmap, worst-case scan.

### Intrusive list

**Plain:** the stored object carries its own links. **Precise:** container topology is embedded in `Order::prev/next`; no separate list node owns the object. **Helios:** FIFO within `PriceLevel`. **Related:** stable address, non-owning pointer.

### `std::unordered_map`

**Plain:** hash-based key lookup container. **Precise:** bucketed associative container with average constant lookup and implementation-controlled nodes/rehash. **Helios:** `OrderId -> Order*`. **Related:** load factor, rehash, collision.

### Hash collision

**Plain:** different keys select the same bucket/probe region. **Precise:** hash values or reduced bucket indexes overlap, requiring equality checks and additional traversal/probing. **Helios:** one reason ID lookup lacks worst-case O(1). **Related:** load factor.

### Load factor / rehash

**Plain:** how full a hash table is / rebuilding it with more buckets. **Precise:** element-to-bucket ratio; growth reallocates bucket structure and redistributes entries. **Helios:** add latency cliff. **Related:** reserve, amortized complexity.

### Object pool

**Plain:** reusable storage for many same-shaped objects. **Precise:** allocator-like owner of blocks and free slots, separating storage acquisition from object construction. **Helios:** stable `Order` allocation. **Related:** placement new, free list.

### Raw storage

**Plain:** memory bytes not yet containing a live object. **Precise:** suitably sized/aligned region in which C++ object lifetime has not begun. **Helios:** pool slot union. **Related:** placement new, destructor.

### Placement new

**Plain:** construct an object at a chosen address. **Precise:** begins a `T` object's lifetime in supplied storage without allocating that storage. **Helios:** pool `acquire`. **Related:** raw storage, exception safety.

### Free list

**Plain:** chain of reusable slots. **Precise:** intrusive stack/list of currently unoccupied pool slots. **Helios:** `Slot::next` while no `Order` is live there. **Related:** union, object lifetime.

### Stable address

**Plain:** an object does not move while live. **Precise:** its pointer value remains valid through unrelated insertions/growth. **Helios:** required by ID-map pointers and intrusive neighbors. **Related:** pointer invalidation.

### Owning pointer / non-owning pointer

**Plain:** one is responsible for lifetime; the other merely observes. **Precise:** ownership determines who destroys/deallocates; non-owning validity depends on an external lifetime invariant. **Helios:** pool owns Orders; map/list raw pointers do not. **Related:** dangling pointer.

### Dangling pointer

**Plain:** an address to an object that is no longer alive. **Precise:** using it has undefined behavior. **Helios:** possible if pool release occurs before all map/list links are removed. **Related:** use-after-free.

### Alignment / `alignas(64)`

**Plain:** start objects at addresses divisible by a chosen boundary. **Precise:** C++ alignment requirement affects layout, allocation, and array stride. **Helios:** `Order` occupies/aligned to a nominal cache-line size. **Related:** cache line, false sharing.

### Fixed-point integer

**Plain:** integer with an agreed implied decimal scale. **Precise:** exact integral ticks interpreted via scale, e.g. raw 10001 means $1.0001 for Price(4). **Helios:** `Price` should carry exact protocol units. **Related:** floating point, strong type.

### Type alias

**Plain:** a readable second name for a type. **Precise:** `using` declaration with no runtime distinction from its underlying type. **Helios:** `OrderId`, `Price`, `Quantity`. **Related:** strong type.

### Strong type

**Plain:** a wrapper preventing unlike units from mixing accidentally. **Precise:** distinct C++ type with constrained construction/operations. **Helios:** not currently used for cents versus Price(4), contributing to HEL-001. **Related:** type alias, dimensional analysis.

### Precondition / postcondition

**Plain:** what must be true before / is guaranteed after a call. **Precise:** function contract constraining inputs and observable state transition. **Helios:** valid range/positive quantity before Add; consistent structures after accepted mutation. **Related:** invariant.

### Invariant

**Plain:** a rule that must always remain true between operations. **Precise:** predicate over object state preserved by every public transition. **Helios:** map↔queue bijection, totals, bitmap/cache, counts, pool liveness. **Related:** transaction, assertion.

### Exception safety

**Plain:** what remains true if an operation fails midway. **Precise:** no-throw, strong (no state change), or basic (valid but changed) guarantees. **Helios:** Add lacks a complete multi-structure rollback proof. **Related:** RAII, scope guard.

### RAII

**Plain:** tie resource lifetime to object lifetime. **Precise:** acquisition during construction and deterministic release during destruction. **Helios:** vectors/maps/mapping wrapper patterns; pool's generic live-object destruction is incomplete. **Related:** ownership.

### Translation unit

**Plain:** one source file after preprocessing, compiled separately. **Precise:** unit to which compiler optimization flags apply before linking. **Helios:** library `.cpp` files and benchmark `.cpp` files may receive different flags. **Related:** linker, CMake target.

## CPU, operating-system, and performance terms

### Cache line

**Plain:** block moved between memory and cache together. **Precise:** hardware coherence/cache transfer granule, commonly 64 bytes on studied x86 systems but platform-specific. **Helios:** `Order` alignment and ladder locality reasoning. **Related:** false sharing.

### Spatial / temporal locality

**Plain:** nearby data / recently used data tends to be used again. **Precise:** access patterns that improve cache-line and cache-residency reuse. **Helios:** direct ladders have spatial locality; random ID-to-order access weakens it. **Related:** working set.

### Working set

**Plain:** data actively needed during a time interval. **Precise:** memory pages/cache lines whose reuse determines cache/TLB pressure. **Helios:** grows during add-only benchmarks. **Related:** cache capacity, RSS.

### CPU pipeline

**Plain:** overlapping stages execute instructions. **Precise:** front end, scheduling, execution, retirement, and recovery allow multiple in-flight operations subject to dependencies/resources. **Helios:** dependent hash/list loads and branch misses limit overlap. **Related:** latency, throughput.

### Instruction latency / throughput

**Plain:** time until one result is ready / rate many can start or finish. **Precise:** dependency-chain delay versus reciprocal issue/execute capacity. **Helios:** pointer chains expose latency; independent decoding may exploit throughput. **Related:** memory-level parallelism.

### Branch prediction

**Plain:** CPU guesses which control path comes next. **Precise:** speculative direction/target prediction; misses flush wrong-path work. **Helios:** validity checks, empty-level transitions, random operation mixes. **Related:** branchless code.

### Dependent load / pointer chasing

**Plain:** the next address is known only after the previous memory read. **Precise:** serialized load-to-use chain limits memory-level parallelism. **Helios:** hash bucket to node to `Order*`, then intrusive links. **Related:** cache miss.

### TLB

**Plain:** cache translating virtual pages to physical pages. **Precise:** translation lookaside buffer; misses trigger page-table walks. **Helios:** large ladders and scattered pool/hash pages expand translation working set. **Related:** page, huge page.

### Page fault

**Plain:** first/invalid page access needs OS handling. **Precise:** exception resolving a virtual mapping; minor faults need no storage I/O, major faults generally do. **Helios:** mmap replay and pool/vector initialization. **Related:** prefault, residency.

### `mmap`

**Plain:** expose file contents through virtual memory addresses. **Precise:** creates a virtual mapping whose pages are populated according to OS demand/policy. **Helios:** replay ingestion. **Related:** page fault, `madvise`.

### Prefetch

**Plain:** request data before it is needed. **Precise:** hardware or software movement/hint intended to overlap miss latency with useful work. **Helios:** possible for sequential frames/sweeps, difficult for random cancellation. **Related:** memory-level parallelism.

### False sharing

**Plain:** threads fight over one cache line despite touching different fields. **Precise:** coherence invalidation caused by independent writable data in the same line. **Helios:** largely irrelevant inside the current single-writer book; alignment alone does not prove benefit. **Related:** cache coherence.

### NUMA

**Plain:** memory is faster from some CPU sockets than others. **Precise:** non-uniform memory access topology with node-local allocation and remote interconnect costs. **Helios:** future multi-shard placement concern, not currently measured. **Related:** affinity, first touch.

### CPU affinity / pinning

**Plain:** restrict a thread to selected cores. **Precise:** OS scheduling mask intended to reduce migration, checked through platform APIs. **Helios:** benchmark utility attempts affinity but inadequately validates it. **Related:** core migration.

### RDTSC / RDTSCP

**Plain:** x86 instructions that read a timestamp counter. **Precise:** architectural counter reads with different ordering/auxiliary semantics; neither replaces a complete serialization and platform-validation method. **Helios:** cycle timer. **Related:** invariant TSC, fence.

### Fence / serialization

**Plain:** constrain reordering around measurement. **Precise:** architectural/compiler ordering mechanism with instruction-specific guarantees. **Helios:** timer boundaries. **Related:** out-of-order execution.

### Invariant TSC

**Plain:** timestamp counter advances at a stable reference rate independent of ordinary frequency changes. **Precise:** platform capability reported via architectural mechanisms; synchronization/virtualization still matter. **Helios:** assumed more than verified. **Related:** calibration.

### Amortized complexity

**Plain:** rare expensive work averaged across many cheap operations. **Precise:** bound on sequence cost, not each individual latency. **Helios:** hash rehash and pool growth. **Related:** worst case, tail latency.

### Hot path / cold path

**Plain:** frequently performance-critical work / rare setup or error work. **Precise:** workload-defined execution regions; labels require profiling. **Helios:** ordinary mutation versus growth, diagnostics, malformed input. **Related:** fast path, slow path.

## Measurement, testing, and build terms

### Benchmark boundary

**Plain:** exact start and finish of what is timed. **Precise:** code interval whose elapsed statistic includes all work between timer observations. **Helios:** operation microbenchmarks and replay process timing. **Related:** setup exclusion.

### Warm-up

**Plain:** run before recording so one-time effects settle. **Precise:** controlled preconditioning of code, data, allocator, caches, branch predictors, and frequency state. **Helios:** benchmark methodology concern. **Related:** steady state, cold start.

### Percentile

**Plain:** threshold below which a fraction of samples falls. **Precise:** empirical order statistic with estimator/sample-size choices. **Helios:** latency reporting. **Related:** p99, tail latency.

### Tail latency

**Plain:** rare slow observations. **Precise:** upper distribution region under a specified workload/time/environment, not a universal bound. **Helios:** claims affected by growth, rehash, faults, scheduler, and timer methodology. **Related:** maximum.

### Provenance / manifest

**Plain:** record of where a result came from. **Precise:** source revision/dirty state, compiler/version/flags, binary/input hashes, hardware, OS, command, environment, and methodology. **Helios:** missing from historical artifacts. **Related:** reproducibility.

### Observer effect

**Plain:** measuring changes the thing measured. **Precise:** timer, logging, counters, allocation hooks, and sampling add work or alter layout/cache state. **Helios:** per-operation cycle reads and profiling drivers. **Related:** batch timing.

### Unit test

**Plain:** checks one small behavior. **Precise:** deterministic test of a narrow contract with controlled inputs and an independent expected result. **Helios:** GTest order-book cases. **Related:** integration test.

### Property test

**Plain:** checks a rule across many generated examples. **Precise:** generator plus invariant/property oracle and shrinking of failures. **Helios:** proposed mutation/invariant verification. **Related:** randomized testing.

### Differential test / reference model

**Plain:** compare fast code with slow obviously-readable code. **Precise:** execute the same operation sequence against independent implementations and compare externally meaningful state. **Helios:** proposed map/deque oracle. **Related:** golden fixture.

### Fuzzing

**Plain:** automatically mutate inputs to find crashes and contract violations. **Precise:** coverage-guided or generative exploration, often with sanitizers and minimized reproducers. **Helios:** proposed for framing/decoding and operation sequences. **Related:** malformed input.

### Sanitizer

**Plain:** instrumented build that detects runtime mistakes. **Precise:** compiler/runtime tooling such as AddressSanitizer, UndefinedBehaviorSanitizer, and ThreadSanitizer. **Helios:** missing automated configurations. **Related:** undefined behavior.

### Golden fixture

**Plain:** known input with a trusted expected output. **Precise:** versioned protocol bytes and state/counter results validated independently. **Helios:** proposed exact ITCH decoder/replay tests. **Related:** deterministic state hash.

### Deterministic state hash

**Plain:** compact fingerprint of canonical book state. **Precise:** stable serialization/order and hash allowing repeated replay comparison; not a proof against collisions by itself. **Helios:** proposed replay validation. **Related:** snapshot.

### CMake target

**Plain:** one buildable library or executable with attached properties. **Precise:** node in CMake's build/usage-requirement graph. **Helios:** library, benchmarks, tests. **Related:** target-scoped flags.

### Debug / Release

**Plain:** development checks/symbols versus optimized build profile. **Precise:** CMake configuration names whose exact flags depend on generator/toolchain/project. **Helios:** benchmark credibility depends on recording the actual compilation commands. **Related:** optimization level.

### `-march=native`

**Plain:** let the compiler use instructions/features of the build machine. **Precise:** GCC/Clang-family architecture selection that can improve local code and reduce binary portability. **Helios:** benchmark flag/provenance issue. **Related:** ISA, compiler flags.

### CI (continuous integration)

**Plain:** automatically build and test changes in clean environments. **Precise:** repeatable pipeline over declared compilers/configurations/platforms. **Helios:** currently absent. **Related:** matrix, reproducibility.

When using any term orally, attach its unit and scope. “Price” should become “raw Price(4) ticks”; “O(1)” should become “constant-time unlink after average-constant hash lookup”; “latency” should become “observed cycle delta for this timed boundary.”
