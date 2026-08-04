# 09 — Testing and Correctness

## What the current suite runs

CMake builds and registers only `tests/test_orderbook.cpp`. The current suite contains seven basic tests and six stress/performance tests. `test_order.cpp`, `test_price_level.cpp`, and `alloc_check.cpp` are not active test targets.

## Basic tests

| Test | Intended | Actually distinguishes | Missing |
|---|---|---|---|
| AddBidOrder | add/index/best/count | one valid bid is visible | level aggregate, duplicate behavior, links |
| AddAskOrder | ask best/count | one valid ask | ID lookup and internals |
| BestBidAsk | extreme prices/spread | cached best after four adds | removal refresh, empty/crossed cases |
| CancelOrder | remove only bid | ID disappears and best becomes zero | counts, pool, links, aggregates |
| FullFill | consume exact best ask | next price becomes best | FIFO identity and all invariants |
| PartialFill | consume available amount | return fill and empty ask | partial head remaining quantity |
| FIFOOrdering | intended FIFO | only cancellation of middle ID | execution order; test name overclaims |

## Stress tests

### Insert100KOrders

Asserts total count and nonzero side-level counts. Printed timing is not a correctness assertion.

### AddCancelMixed

Checks `total == adds - successful cancels`. It does not inspect queue topology, level totals, or bitmap consistency.

### MarketOrderExecution

Prints results without substantive final assertions.

### LatencyDistribution

Collects clock samples inside a test executable. It verifies no latency requirement and can print zeros on coarse clock behavior.

### OneMillionOrders

Checks only total count. The resulting synthetic book is crossed because adds do not match; that is allowed by current storage semantics but should be explicit.

### InvariantsHoldUnderStress

The primary count assertion compares `getTotalOrders()` to itself through a conditional whose both branches are the same. It cannot detect inconsistency. Later best-price checks are weak but non-tautological.

## False confidence patterns

- Large operation count is not deep state validation.
- A test named FIFO does not prove FIFO unless it observes consumption order.
- Successful replay with a plausible best bid/ask does not prove protocol correctness.
- Sanitizers passing existing tests only cover executed paths.
- Printed performance in a test is not a performance contract.

## Verification techniques

### Unit tests

Focused examples for one component/contract: endian readers, pool reuse, level unlink topology, zero quantity, duplicate IDs.

### Invariant testing

Expose or friend a debug validator that reconstructs counts/aggregates and compares every redundant representation.

### Property testing

Generate many valid operations and assert properties independent of exact sequence, such as “canceling a live ID reduces total by one.”

### Differential testing

Run the same operation sequence against Helios and a deliberately slow reference model using maps and deques. Compare externally visible full state after every operation.

### Golden fixtures

Fixed official/spec-derived byte arrays with exact expected decoded fields and mutation snapshots.

### Fuzzing

Generate arbitrary/truncated frames and assert no out-of-bounds access, monotonic parser progress, and explicit status.

### Sanitizers

- ASan: invalid memory access/use-after-free;
- UBSan: selected undefined behavior;
- potential future MSan/TSan depending on platform and concurrency.

## Reference model design

The oracle should optimize clarity, not speed:

```text
map<ID, ReferenceOrder>
map<Price, deque<ID>, descending bids>
map<Price, deque<ID>, ascending asks>
```

After each operation compare:

- set of live IDs and fields;
- order sequence at every price;
- aggregate quantities;
- best bid/ask/spread;
- accepted/rejected result;
- total live count.

The reference must implement the intended contract, not copy Helios implementation details.

## Generated sequence example

```text
add B id=1 p=100 q=10
add B id=2 p=100 q=20
reduce id=1 by 4
cancel id=2
add S id=3 p=102 q=5
synthetic buy q=3
replace id=1 -> id=4 p=99 q=8
```

After every event, compare both models and run the complete invariant set from [04](04-data-structures-and-invariants.md).

## Replay verification

For a known input:

- hash normalized event stream;
- count all framed and each decoded type;
- count target events and successful mutations;
- record anomaly counters;
- hash sorted final orders and per-level FIFO sequence;
- compare with an independent parser/model.

## Designed but not implemented verification roadmap

1. Golden endian/message fixtures.
2. Direct `PriceLevel` topology tests.
3. Direct `ObjectPool` lifecycle/reuse tests.
4. Duplicate/zero/range/numeric unit tests.
5. Full invariant validator.
6. Reference differential random sequences.
7. Parser fuzzing and sanitizer CI.
8. Real-file state hashes and independent comparison.

