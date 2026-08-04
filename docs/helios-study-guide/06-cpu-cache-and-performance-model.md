# 06 — CPU, Cache, and Performance Model

## Evidence discipline

This chapter labels conclusions carefully. Source structure can suggest likely bottlenecks, but only controlled measurement on named hardware establishes them.

Primary references used for architectural/OS claims:

- **Externally verified fact:** Intel defines the architectural behavior of caches, ordering, fences, RDTSC, and RDTSCP in the [Intel 64 and IA-32 Software Developer's Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html).
- **Externally verified fact:** Linux documents CPU isolation facilities and their limitations in the [kernel CPU isolation guide](https://docs.kernel.org/admin-guide/cpu-isolation.html).
- **Externally verified fact:** Linux `mmap` establishes mappings and `madvise` supplies access advice; neither source-level call alone proves residency or absence of later faults. See [`mmap(2)`](https://man7.org/linux/man-pages/man2/munmap.2.html) and [`madvise(2)`](https://man7.org/linux/man-pages/man2/madvise.2.html).

Cache sizes, line size, TLB organization, instruction costs, counter stability, and NUMA topology are properties of the named target system. This chapter does not infer their exact values from “x86.”

## CPU pipeline

### Intuition

A modern core overlaps fetch, decode, execution, and retirement for many instructions. Independent work can proceed in parallel; dependent work forms a chain.

### Helios example

Following `head->next` is dependent: the next address is unknown until the current node loads. Hash lookup similarly creates bucket→node→order dependencies.

## Cache hierarchy and cache lines

Caches keep recently used memory closer to the core. Data transfers in fixed-size lines, commonly 64 bytes on relevant x86 systems.

- **Temporal locality:** reuse the same data soon.
- **Spatial locality:** use nearby addresses soon.

`Order` is forced to 64 bytes and 64-byte alignment. **Repository evidence:** every order begins on its own aligned region. **Engineering inference:** this avoids split lines but reduces density and increases TLB/cache footprint. Whether it wins requires BEN-10-style measurement.

## TLBs and pages

The TLB caches virtual-to-physical translations. A working set spread over many pages can miss in caches even when individual data accesses are otherwise efficient.

Replay combines a large file mapping, dense ladders, a large pool, hash buckets, and scattered hash nodes. **Engineering inference:** TLB behavior may matter; saved artifacts do not isolate DTLB events.

## Branch prediction

The CPU predicts branch direction to keep the pipeline busy. A wrong prediction discards speculative work.

Likely Helios patterns:

- range check: usually success;
- level-empty transition: rare after warm-up;
- side branch: workload dependent;
- full versus partial fill: distribution dependent;
- message switch: type distribution dependent;
- hash allocator internals: complex.

The add benchmark changes predictability by pre-occupying used bid levels.

## Memory-level parallelism

Independent cache misses can overlap. Pointer chains cannot. A market sweep’s head→next chain and node-based hash traversal limit overlap. Direct ladder address computation is independent and predictable.

## Allocation overhead

An allocator may manipulate metadata, synchronize, request pages, or touch new memory. Pool allocate is a few dependent pointer operations when capacity exists, but hash-node allocation remains. Pool growth and rehash create different slow paths.

## Page faults

A minor fault establishes mapping/page-table state without disk I/O; a major fault requires storage access. `mmap` defers some work until access unless prefault/read-ahead succeeds.

The pool constructor writes every slot while building its free list. Therefore the historical explanation that later adds first-touch untouched initial pool pages conflicts with current source.

## False sharing

False sharing occurs when different threads mutate distinct values on the same cache line, forcing coherence traffic. Helios is single-threaded, so this is not a current order-node problem. If shared without synchronization, data races are the first problem; false sharing is secondary.

## Instruction latency versus throughput

- **Latency:** time until an instruction’s result is available to a dependent instruction.
- **Throughput:** rate at which independent instances can execute.

RDTSC fences add serialization latency. A batch loop can overlap independent work across iterations more than a fence-around-each-operation benchmark.

## NUMA

On multi-socket systems, memory attached to another NUMA node has higher access cost. Pinning a thread without binding/pre-touching memory on the same node is incomplete placement control. Current saved environment does not document NUMA.

## Prefetching

Hardware prefetchers recognize predictable streams such as sequential parsing. Manual prefetch may help when a future address is known early enough, such as `head->next` in a long sweep, but it also consumes instructions/cache bandwidth.

Do not propose prefetch as a fact. Form a hypothesis, identify stall counters, control the workload, and compare variants.

## Structure-by-structure performance model

| Structure/path | Locality advantage | Likely cost | Evidence status |
|---|---|---|---|
| Direct ladder | contiguous, computed address | large reserved footprint | source fact; benefit historically measured but provenance incomplete |
| Bitmap | compact words | global endpoint scan | source fact |
| Pool free list | recently reused LIFO slots | dependent load; growth cliff | source fact |
| Intrusive FIFO | no list-node allocation | pointer chasing | source fact |
| Hash index | average direct identity lookup | bucket/node/allocator indirection | source fact |
| Symbol compare | fixed eight-byte compare | parser still copies/trim symbols | source fact |
| Parser | sequential bytes | switch and field assembly | source fact |
| Market sweep | head locality may reuse hot orders | serialized traversal + hash erase | inference requiring counters |
| Random cancel | exact pointer after hash | scattered node/order/neighbors | inference requiring counters |

## Saved measurements

### Repository evidence

Tracked artifacts report a historical add batch mean of 26.6 ns and 37.57M adds/s for an array-ladder variant on one x86/Linux environment.

### Historical/unverified limits

The artifact does not include enough source/build/binary/hardware provenance to reproduce it exactly. It measures the primary add workload, not universal book operations.

### Unsupported conclusion

Aggregate IPC/cache/page-fault counters do not prove the approximately 24 µs recorded tail was non-algorithmic. Rare-path correlation is missing.

## How to test optimization claims

1. State the mechanism before measuring.
2. Define a workload that exercises the mechanism.
3. Keep source semantics and build flags controlled.
4. preserve raw samples and counters.
5. record source, binary hash, compiler, CPU, kernel, affinity, and topology.
6. publish counterevidence and inconclusive results.

## Interview-level summary

Helios replaces tree traversal with direct address computation and pools order storage, but its hot path still contains node allocation and pointer-rich indexing. Cache-line alignment, prefetching, and alternative hashes are hypotheses; the strongest demonstrated design story is the direct-ladder experiment, subject to artifact-provenance limitations.
