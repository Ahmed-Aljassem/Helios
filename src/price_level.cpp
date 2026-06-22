#include "price_level.hpp"

namespace orderbook {

void PriceLevel::addOrder(Order* o) {
    if (!o) return;

    o->prev = tail_;
    o->next = nullptr;

    if (tail_) {
        tail_->next = o;
    } else {
        head_ = o;
    }

    tail_ = o;
    total_qty_ += o->quantity;
    ++order_count_;
}

void PriceLevel::removeOrder(Order* o) {
    if (!o) return;

    Order* prev = o->prev;
    Order* next = o->next;

    if (prev) {
        prev->next = next;
    } else {
        head_ = next;
    }

    if (next) {
        next->prev = prev;
    } else {
        tail_ = prev;
    }

    total_qty_ -= o->quantity;
    --order_count_;

    o->prev = nullptr;
    o->next = nullptr;
}

void PriceLevel::reduceHeadQuantity(Quantity qty) {
    if (!head_ || qty > head_->quantity) return;

    head_->quantity -= qty;
    total_qty_ -= qty;

    if (head_->quantity == 0) {
        removeOrder(head_);
    }
}

void PriceLevel::adjustTotalQuantity(int64_t delta) {
    if (delta >= 0) {
        total_qty_ += static_cast<Quantity>(delta);
    } else {
        total_qty_ -= static_cast<Quantity>(-(delta + 1)) + 1;
    }
}

} // namespace orderbook
