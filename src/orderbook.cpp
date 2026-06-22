#include "orderbook.hpp"
#include <algorithm>

namespace orderbook {

OrderId OrderBook::addOrder(Side side, Price price, Quantity quantity, OrderId id) {
    if (!inRange(price)) return 0;
    int64_t idx = priceToIndex(price);
    Order* o = order_pool_.allocate(id, side, price, quantity);
    orders_[o->id] = o;
    if (side == Side::BUY) {
        bool wasEmpty = bid_levels_[idx].isEmpty();
        bid_levels_[idx].addOrder(o);
        if (wasEmpty) { 
            setBit(bid_words_, idx); 
            ++bid_count_;
            if (best_bid_idx_ < 0 || idx > best_bid_idx_) best_bid_idx_ = idx; 
        }
    } else {
        bool wasEmpty = ask_levels_[idx].isEmpty();
        ask_levels_[idx].addOrder(o);
        if (wasEmpty) { 
            setBit(ask_words_, idx); 
            ++ask_count_;
            if (best_ask_idx_ < 0 || idx < best_ask_idx_) best_ask_idx_ = idx; 
        }
    }
    ++total_orders_;
    return o->id;
}

bool OrderBook::cancelOrder(OrderId id) {
    auto it = orders_.find(id);
    if (it == orders_.end()) return false;
    Order* o = it->second;
    int64_t idx = priceToIndex(o->price);
    if (o->side == Side::BUY) {
        bid_levels_[idx].removeOrder(o);
        if (bid_levels_[idx].isEmpty()) { 
            clearBit(bid_words_, idx); 
            --bid_count_;
            if (idx == best_bid_idx_) best_bid_idx_ = highestSetBit(bid_words_); 
        }
    } else {
        ask_levels_[idx].removeOrder(o);
        if (ask_levels_[idx].isEmpty()) { 
            clearBit(ask_words_, idx); 
            --ask_count_;
            if (idx == best_ask_idx_) best_ask_idx_ = lowestSetBit(ask_words_); 
        }
    }
    orders_.erase(it);
    order_pool_.deallocate(o);
    --total_orders_;
    return true;
}

Quantity OrderBook::executeMarketOrder(Side side, Quantity quantity) {
    Quantity remaining = quantity;
    if (side == Side::BUY) {
        while (remaining > 0 && best_ask_idx_ >= 0) {
            PriceLevel& lvl = ask_levels_[best_ask_idx_];
            Quantity fill = std::min(remaining, lvl.getTotalQuantity());
            Quantity filled = 0;
            while (filled < fill) {
                Order* h = lvl.getHead(); 
                if (!h) break;
                Quantity tf = std::min(h->quantity, fill - filled);
                if (tf == h->quantity) { 
                    // OPTIMIZATION: Direct O(1) bypass instead of calling heavy cancelOrder()
                    orders_.erase(h->id);
                    lvl.removeOrder(h);
                    order_pool_.deallocate(h);
                    --total_orders_;
                } else {
                    lvl.reduceHeadQuantity(tf);
                }
                filled += tf;
            }
            remaining -= filled;
            if (lvl.isEmpty()) {
                clearBit(ask_words_, best_ask_idx_);
                --ask_count_;
                best_ask_idx_ = lowestSetBit(ask_words_);
            }
            if (filled == 0) break;
        }
    } else {
        while (remaining > 0 && best_bid_idx_ >= 0) {
            PriceLevel& lvl = bid_levels_[best_bid_idx_];
            Quantity fill = std::min(remaining, lvl.getTotalQuantity());
            Quantity filled = 0;
            while (filled < fill) {
                Order* h = lvl.getHead(); 
                if (!h) break;
                Quantity tf = std::min(h->quantity, fill - filled);
                if (tf == h->quantity) { 
                    // OPTIMIZATION: Direct O(1) bypass instead of calling heavy cancelOrder()
                    orders_.erase(h->id);
                    lvl.removeOrder(h);
                    order_pool_.deallocate(h);
                    --total_orders_;
                } else {
                    lvl.reduceHeadQuantity(tf);
                }
                filled += tf;
            }
            remaining -= filled;
            if (lvl.isEmpty()) {
                clearBit(bid_words_, best_bid_idx_);
                --bid_count_;
                best_bid_idx_ = highestSetBit(bid_words_);
            }
            if (filled == 0) break;
        }
    }
    return quantity - remaining;
}

bool OrderBook::modifyOrder(OrderId id, Quantity new_quantity) {
    auto it = orders_.find(id);
    if (it == orders_.end()) return false;
    Order* o = it->second;
    int64_t idx = priceToIndex(o->price);
    int64_t delta = (int64_t)new_quantity - (int64_t)o->quantity;
    o->quantity = new_quantity;
    if (o->side == Side::BUY) bid_levels_[idx].adjustTotalQuantity(delta);
    else                      ask_levels_[idx].adjustTotalQuantity(delta);
    return true;
}

void OrderBook::printBook(int depth) const {
    // Presentation stub: can be filled later to print market depth
    (void)depth; 
}

} // namespace orderbook
