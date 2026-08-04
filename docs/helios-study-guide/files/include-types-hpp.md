# File Guide: `include/types.hpp`

## 1. File purpose

Defines the project's domain vocabulary: side, order type, price, quantity, identity, timestamp, and display/conversion helpers.

## 2. Architecture position

This is the lowest-level domain header and is included transitively by nearly all book structures.

## 3. Dependencies

`<cstdint>` for widths and `<string>` for formatting.

## 4. Symbols defined

`Side`, `OrderType`, aliases `Price`, `Quantity`, `OrderId`, `Timestamp`, plus `priceToString` and `dollarToPrice`.

## 5. Symbols consumed

All order, level, book, replay, benchmark, and test code consumes these types.

## 6. Source-order walkthrough

- **Lines 1–6:** header guard and `orderbook` namespace.
- **Lines 8–18:** byte-sized scoped enums prevent accidental implicit integer conversion. `OrderType::MARKET` exists even though synthetic market orders are not stored as resting `Order` objects in the same way.
- **Lines 20–32:** aliases choose signed 64-bit price and unsigned 64-bit quantity/ID/time. Aliases improve readability but do not prevent mixing same-underlying-type units.
- **Lines 34–39:** formatting computes a floating divisor and delegates to `std::to_string`, yielding implementation-style fixed digits rather than a carefully specified market-price formatter.
- **Lines 41–47:** dollar conversion multiplies binary floating point and truncates on cast; rounding and range policy are absent (HEL-019).

## 7. Inputs and outputs

Formatting accepts integer ticks and decimal count, returning text. Conversion accepts `double` dollars and returns integral ticks.

## 8. Ownership and lifetime

Enums/integers own no resources. Returned `std::string` owns its buffer.

## 9. Invariants

A `Price` is meaningful only with a scale. Quantity should be nonnegative by type, but zero still needs semantic policy. Live `OrderId` uniqueness is external.

## 10. Errors and edge cases

Negative decimals cause no loop; large decimals can overflow multiplier; floating products can truncate unexpectedly; negative dollars are representable despite book ranges.

## 11. Performance implications

Aliases are zero-cost. String/float conversion is cold-path work and may allocate; it should not be part of mutation timing.

## 12. Technical debt

The cents commentary conflicts with ITCH Price(4) replay needs. Strong price/unit types would prevent HEL-001-class mistakes.

## 13. Related findings

HEL-001, HEL-010, HEL-019, HEL-041, HEL-042.

## 14. Interview questions

- Why is an alias not dimensional type safety?
- Why is integer fixed point correct only if scale is preserved?
- What rounding contract should decimal ingestion use?

## 15. Exercises

List every price unit in the repository and write an explicit conversion graph. Find decimal inputs for which truncation differs from round-to-nearest.
