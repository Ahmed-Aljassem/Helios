#pragma once
#include "order.hpp"
#include "types.hpp"
#include <cstddef>
#include <cstdint>

namespace orderbook {

class PriceLevel {
public:
    PriceLevel() : price_(0), head_(nullptr), tail_(nullptr), total_qty_(0), order_count_(0) {}

    void setPrice(Price p) { price_ = p; }
    Price getPrice() const { return price_; }
    Order* getHead() const { return head_; }
    Order* getTail() const { return tail_; }
    Quantity getTotalQuantity() const { return total_qty_; }
    size_t getOrderCount() const { return order_count_; }
    bool isEmpty() const { return head_ == nullptr; }

    void addOrder(Order* o);
    void removeOrder(Order* o);
    void reduceHeadQuantity(Quantity qty);
    void adjustTotalQuantity(int64_t delta);

private:
    Price price_;
    Order* head_;
    Order* tail_;
    Quantity total_qty_;
    size_t order_count_;
};

} // namespace orderbook
