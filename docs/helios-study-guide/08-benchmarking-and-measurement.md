# 08 — Benchmarking and Measurement

## What a benchmark must answer

A useful result names:

- operation and state distribution;
- exact timed boundary;
- excluded setup;
- compiler/build/binary;
- hardware/OS placement;
- statistic and raw samples;
- scope of the supported conclusion.

## TSC, RDTSC, and RDTSCP

The timestamp counter is a processor counter used as a high-resolution timebase on supported x86 systems. `RDTSC` reads it; `RDTSCP` also returns an auxiliary value often used to identify CPU context and has ordering properties intended for measurement sequences.

Helios uses:

```text
start: LFENCE → RDTSC → LFENCE
end:   RDTSCP → LFENCE
```

**Externally verified source:** Intel instruction and architecture behavior is defined by the Intel Software Developer’s Manuals: <https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html>

The code assumes relevant capabilities instead of checking CPUID, and discards RDTSCP AUX.

## Invariant TSC and frequency

An invariant TSC advances at a stable reference rate rather than current core turbo frequency. Helios calibrates ticks/second against `CLOCK_MONOTONIC` over roughly 200 ms. The printed “TSC GHz” is conversion frequency, not proof that the core executed instructions at that GHz.

## CPU pinning

Pinning constrains eligibility to a core and reduces migration. The benchmark ignores `sched_setaffinity`’s return value and reports the requested core. Linux can reject or constrain affinity under cpusets. Pinning does not isolate interrupts, SMT siblings, scheduler tick, firmware, or power behavior.

**Externally verified fact:** Linux specifies the affinity mask, return status, cpuset interaction, and possible errors in [`sched_setaffinity(2)`](https://man7.org/linux/man-pages/man2/sched_setaffinity.2.html). The repository does not check that contract.

## Warm-up

Warm-up may stabilize code pages, caches, predictors, allocation state, and frequency. It also defines the measured state distribution.

The primary benchmark inserts one bid at every used price. This biases later operations toward already occupied levels and excludes many bitmap/best transitions.

## Observer effect

Measuring a very short operation changes it. Fences and counter reads serialize execution and may cost a substantial fraction of the operation. Batch timing perturbs less per operation but yields an aggregate rather than individual distribution.

## Timer overhead subtraction

Helios takes the minimum empty start/end pair and subtracts it from every per-op sample.

Why that is not automatically valid:

- timer overhead varies;
- fences change overlap and pipeline behavior;
- operation and timing sequence are not necessarily separable additive costs;
- subtracting a minimum biases estimates downward;
- short samples clamp to zero.

Report observed instrumented latency separately unless a subtraction model is justified.

## Percentiles and maxima

A p99 is a value at or below which about 99% of samples fall under the chosen quantile definition. A maximum is the greatest observed sample in a specified sample set.

The benchmark takes seven per-run maxima and reports their median. That is a median-of-run-maxima, not the global maximum or “worst single sample.”

## Primary benchmark walkthrough

### Input generation

`makeInputs` uses fixed-seed `mt19937` and uniform side, price, and quantity distributions. RNG is excluded from timed add regions.

### Batch path

`batchThroughputNs` constructs a book with a one-million-order pool, pre-adds every price from 9000 to 11000 on the bid side, then times only the loop calling `addOrder` and a compiler barrier.

Included:

- pool pop/construction;
- hash node allocation/insertion;
- ladder/list/aggregate work;
- any OS/allocator effects during those adds.

Excluded:

- book/ladders/pool construction;
- map reserve;
- level warm-up;
- input RNG;
- destruction.

The live set grows by 200,000 and is never canceled.

### Per-operation path

Each of seven runs constructs a new book, adds used bid levels, performs 20,000 warm-up adds, then fences/times each of 200,000 further adds. Samples are sorted; selected quantiles are collected; medians across runs are printed.

### What 26.6 ns proves

**Historical/unverified claim:** one saved artifact reports 26.6 ns batch mean for this add workload on a recorded x86/Linux environment. It does not prove:

- cancel latency;
- mixed workload throughput;
- replay latency;
- allocation-free operation;
- worst-case latency;
- portability to other CPUs;
- current-source reproducibility.

## `profile_driver.cpp`

The loop chooses actions using RNG, randomly selects a live ID, updates a vector, calls book methods, and accumulates a sink. Whole-program perf counters include all that work. The result is valid as end-to-end synthetic-driver behavior, not isolated book IPC/cache behavior.

## `itch_replay.cpp`

Times nothing itself; it maps and parses, counting supported message types. External tools can profile it. Missing CLI/system-call validation can turn invalid invocations into undefined or misleading behavior.

## `itch_book_replay.cpp`

Records load and process milliseconds. Load includes mapping, advice, and page touching. Process begins before `OrderBook` construction, so it includes large book initialization plus parse and mutation.

The throughput numerator is supported decoded messages, while the time includes scanning unmodeled frames.

## `manual_test.cpp` and test timings

These use high-resolution clock around loops that include RNG and workload bookkeeping in several cases. They are demonstrations, not stable microbenchmarks. Performance output has no pass/fail threshold and should not be used as canonical evidence.

## Compile-flag credibility

The current local compile commands show the library at `-O2` and benchmark translation unit additionally at `-O3 -march=native` in a no-build-type configuration. A separately compiled library does not become O3 because the executable source uses O3. GCC also documents that optimization acts at compile time and `-march=native` targets the build host: <https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html> and <https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html>.

## Provenance checklist

- commit and dirty status;
- complete compile/link commands;
- compiler version;
- binary hash;
- CPU model/topology/microcode;
- RAM/NUMA;
- kernel and governor/power policy;
- affinity and isolation status;
- input seed/file hash;
- raw samples/counters;
- exact quantile calculation;
- repeated runs and stability.

## Interview answer

“The saved 26.6 ns is a historical batch-add result for a specific synthetic state distribution. The harness includes several good controls, but current artifact provenance and workload scope are insufficient for a universal latency claim. I would preserve raw samples, verify build/platform prerequisites, bound live state, and benchmark each lifecycle path separately.”
