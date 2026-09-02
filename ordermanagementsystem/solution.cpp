// Solution. Build: g++ -std=c++17 -o solution solution.cpp && ./solution

#include <cassert>
#include <iostream>
#include <optional>

enum class Side { Buy, Sell };

struct Quote {
  Side side;
  long px;
  long qty;
  bool operator==(const Quote& o) const {
    return side == o.side && px == o.px && qty == o.qty;
  }
};

struct Exchange {
  virtual ~Exchange() = default;
  virtual void sendOrder(long id, Side, long px, long qty) = 0;
  virtual void cancelOrder(long id) = 0;
};

class OrderManager {
 public:
  enum class State { None, PendingNew, Live, PendingCancel };

  explicit OrderManager(Exchange& ex) : ex_(ex) {}

  // Trader quotes. Latest wins; qty 0 means pull.
  void onQuote(Side s, long px, long qty) {
    if (qty > 0) desired_ = Quote{s, px, qty};
    else         desired_.reset();
    reconcile();
  }

  void onSendAck(long id, bool accepted) {
    if (id != id_) return;
    if (accepted) {
      state_ = State::Live;
    } else {
      state_ = State::None;
      desired_.reset();          // else we resend the same rejected quote forever
    }
    reconcile();
  }

  void onCancelAck(long id, bool accepted) {
    if (id != id_) return;
    state_ = accepted ? State::None : State::Live;
    reconcile();
  }

  void onFill(long id, long qty) {
    if (id != id_) return;
    left_ -= qty;
    if (left_ <= 0) state_ = State::None;
    reconcile();
  }

  State state() const { return state_; }

 private:
  void reconcile() {
    if (state_ == State::None) {
      if (desired_) send(*desired_);
    } else if (state_ == State::Live) {
      if (!desired_ || !(*desired_ == Quote{sent_.side, sent_.px, left_})) cancel();
    }
    // PendingNew / PendingCancel: nothing may be sent. The ack re-runs this check.
  }

  void send(const Quote& q) {
    id_ = ++seq_;
    sent_ = q;
    left_ = q.qty;
    state_ = State::PendingNew;
    ex_.sendOrder(id_, q.side, q.px, q.qty);
  }

  void cancel() {
    state_ = State::PendingCancel;
    ex_.cancelOrder(id_);
  }

  Exchange& ex_;
  std::optional<Quote> desired_;        // what the trader wants
  Quote sent_{Side::Buy, 0, 0};         // what the exchange has
  State state_ = State::None;
  long  id_ = 0, seq_ = 0, left_ = 0;
};

// ------------------------------------------------------------------- strategy

// Strategies only express intent. No ids, no acks, no "am I allowed to send".
class JoinBidStrategy {
 public:
  JoinBidStrategy(OrderManager& oms, long size) : oms_(oms), size_(size) {}

  void onBook(long bid, long /*ask*/) { oms_.onQuote(Side::Buy, bid, size_); }
  void onPositionLimitHit()           { oms_.onQuote(Side::Buy, 0, 0); }

 private:
  OrderManager& oms_;
  long size_;
};

// ------------------------------------------------------------------ scratch test

struct Mock : Exchange {
  int news = 0, cancels = 0;
  long lastPx = 0, lastQty = 0;
  bool live = false;

  void sendOrder(long id, Side, long px, long qty) override {
    assert(!live && "two live orders");
    live = true;
    ++news;
    lastPx = px;
    lastQty = qty;
    std::cout << "NEW " << id << " " << qty << "@" << px << "\n";
  }
  void cancelOrder(long id) override {
    ++cancels;
    std::cout << "CANCEL " << id << "\n";
  }
};

int main() {
  {  // requote three times while pending: one cancel, then the LATEST quote
    Mock ex;
    OrderManager oms(ex);
    oms.onQuote(Side::Buy, 100, 10);
    oms.onQuote(Side::Buy, 101, 10);
    oms.onQuote(Side::Buy, 102, 20);
    assert(ex.news == 1 && ex.cancels == 0);
    oms.onSendAck(1, true);
    assert(ex.cancels == 1);
    ex.live = false;
    oms.onCancelAck(1, true);
    assert(ex.news == 2 && ex.lastPx == 102 && ex.lastQty == 20);
  }
  {  // identical requote costs nothing; stale ack is ignored
    Mock ex;
    OrderManager oms(ex);
    oms.onQuote(Side::Buy, 100, 10);
    oms.onSendAck(1, true);
    oms.onQuote(Side::Buy, 100, 10);
    assert(ex.cancels == 0);
    oms.onSendAck(99, false);
    assert(oms.state() == OrderManager::State::Live);
  }
  std::cout << "ok\n";
}
