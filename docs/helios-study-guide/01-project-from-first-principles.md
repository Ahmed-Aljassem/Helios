# 01 — Helios from First Principles

## A market in plain English

A market is a mechanism for finding mutually acceptable trades. Some participants announce prices at which they are willing to buy; others announce prices at which they are willing to sell.

- A **bid** is an offer to buy.
- An **ask** (or offer) is an offer to sell.
- The **best bid** is the highest displayed buying price.
- The **best ask** is the lowest displayed selling price.
- The **spread** is `best ask - best bid`.

If the best bid is `$100.00` and the best ask is `$100.05`, the spread is `$0.05`.

## Orders

An **order** is an instruction with identity, side, quantity, and usually price.

- A **limit order** says “buy or sell up to this quantity, but do not accept a worse price than this limit.”
- A **market order** says “trade the requested quantity against the best available opposite-side liquidity now,” subject to the engine’s exact policy.

Helios represents an order with an ID, integer price, integer quantity, side, timestamp, type, and intrusive queue links.

## Price-time priority

Most displayed continuous books prioritize:

1. better price before worse price;
2. earlier order before later order at the same price.

Suppose the ask side is:

| Price | FIFO orders |
|---:|---|
| $100.01 | A: 50, then B: 30 |
| $100.02 | C: 100 |

A market buy for 60 shares consumes all 50 from A and 10 from B. It does not touch C because `$100.01` is the better ask price, and it does not skip B because B is next in time at that level.

## Price levels

A **price level** groups all orders with the same side and price. It stores:

- the price;
- the FIFO head and tail;
- total displayed quantity;
- number of orders.

Grouping makes “what liquidity exists at this price?” different from “where is order reference 927?” Helios therefore needs two index families:

- price → price level;
- order ID → exact order.

## Tiny book simulation

Start empty.

### Event 1: add bid 101, quantity 5, ID 1

```text
BIDS: 101 -> [#1 qty=5]
ASKS: empty
best bid=101, best ask=none
```

Helios allocates an `Order`, indexes ID 1, appends it to bid level 101, sets that level’s bitmap bit, and caches 101 as best bid.

### Event 2: add another bid at 101, ID 2, quantity 7

```text
BIDS: 101 -> [#1 qty=5] <-> [#2 qty=7]
```

ID 2 goes at the tail. Level quantity is 12. The bitmap does not change because the level was already occupied.

### Event 3: add ask 103, ID 3, quantity 4

```text
BIDS: 101 -> [#1:5] [#2:7]
ASKS: 103 -> [#3:4]
spread = 103 - 101 = 2 ticks
```

### Event 4: cancel ID 1

The ID index finds the pointer directly. Intrusive links remove it without scanning the queue.

```text
BIDS: 101 -> [#2:7]
ASKS: 103 -> [#3:4]
```

### Event 5: synthetic market buy quantity 6

The buy consumes asks. Only four shares exist, so ID 3 is fully removed and the returned fill is 4.

```text
BIDS: 101 -> [#2:7]
ASKS: empty
```

## Reconstruction book versus matching engine

### Explain like I am five

A matching engine is the referee deciding which new requests trade. A reconstruction book is a notebook copying decisions that another referee already made.

### Precise model

An exchange matching engine accepts order-entry commands, validates them, chooses trades, assigns priority, generates executions, and publishes resulting market data.

A market-data reconstruction book consumes already-published events and recreates displayed state. ITCH `E`, `D`, and `U` messages report decisions Nasdaq already made.

Helios replay is the second system. Its general `OrderBook` also has `executeMarketOrder`, which is useful for synthetic experiments, but `addOrder` does not match crossing prices. Therefore Helios does not implement a complete aggressive-limit matching path.

### Common misconception

“It stores bids and asks and can execute market orders, so it is a matching engine.” That ignores order entry, validation, aggressive limit matching, execution reports, trade IDs, risk checks, and exchange priority policy.

## What Nasdaq TotalView-ITCH is

**Externally verified fact:** TotalView-ITCH is an outbound binary market-data feed describing order-level displayed state and related market events. Its messages are normally carried by a sequencing/delivery protocol; historical data may be stored in BinaryFILE framing.

Historical replay is useful for:

- reconstructing market state;
- testing parsers and strategies deterministically;
- measuring feed-handler throughput without live-network variability;
- investigating particular sessions;
- validating state-machine behavior against known data.

## What Helios implements

**Repository evidence:**

- single-threaded book state;
- add, cancel, quantity modification, best prices, spread, and synthetic market consumption;
- caller-provided exchange order IDs;
- direct bid/ask price ladders;
- occupancy bitmaps;
- intrusive per-level FIFO queues;
- pooled `Order` storage;
- modeled ITCH `A`, `F`, `E`, `C`, `X`, `D`, and `U` decoding;
- one-symbol replay from an `mmap`-mapped historical file;
- an x86 RDTSC add benchmark;
- GoogleTest and manual/stress workloads.

## What Helios does not implement

- faithful four-decimal ITCH pricing in the current replay;
- live feed sequencing, redundancy, gaps, or recovery;
- full symbol directory and stock-locate routing;
- multi-symbol ownership/sharding;
- order-entry risk or validation policy;
- a full matching engine;
- persistence, snapshots, or restart recovery;
- production observability/backpressure;
- verified allocation-free operation;
- reproducible benchmark artifact manifests;
- kernel bypass, FPGA parsing, or lock-free ingestion.

## Three explanation levels

### Beginner

Helios reads a recording of Nasdaq order changes and keeps a fast notebook of orders. It uses one lookup by order number and another lookup by price. It tries to avoid slow memory allocation for order objects.

### Undergraduate CS

Helios is a deterministic event-driven state machine. It combines a hash table for identity, dense arrays for price, linked queues for time priority, bitsets for occupied prices, and a slab-like pool for stable node storage. A binary parser normalizes network-order fields before a replay adapter mutates the generic book.

### Senior systems engineer

Helios is a single-writer, pointer-rich in-memory index with redundant derived state. Its performance comes from bounding the price domain, amortizing order storage allocation, and avoiding tree traversal; its risks come from domain mismatch, node-based hash allocation, invariant coupling, incomplete error semantics, and benchmark provenance. The direct ladder is a workload-specific choice, not a universal order-book design.

## Self-test

Without notes, answer:

1. Why are ID lookup and price lookup separate problems?
2. Why is cancel O(1) average but not allocation-free?
3. Why can replay ignore an execution’s reported execution price when updating displayed quantity?
4. Why does accepting crossing adds not automatically mean the stored state is corrupt?
5. What additional components would make this a production matching engine?

