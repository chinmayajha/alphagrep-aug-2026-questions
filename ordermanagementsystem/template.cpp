// Skeleton. Fill in the TODOs until main() passes.
// Build: g++ -std=c++17 -o template template.cpp && ./template

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
    (void)s; (void)px; (void)qty;
    // TODO
  }

  void onSendAck(long id, bool accepted) {
    (void)id; (void)accepted;
    // TODO
  }

  void onCancelAck(long id, bool accepted) {
    (void)id; (void)accepted;
    // TODO
  }

  void onFill(long id, long qty) {
    (void)id; (void)qty;
    // TODO
  }

  State state() const { return state_; }

 private:
  // Called after every event below.
  void reconcile() {
    // TODO
  }

  void send(const Quote& q) { (void)q; /* TODO */ }

  void cancel() { /* TODO */ }

  Exchange& ex_;                        // the exchange you call sendOrder()/cancelOrder() on
  std::optional<Quote> desired_;        // what the trader wants
  Quote sent_{Side::Buy, 0, 0};         // what the exchange has
  State state_ = State::None;           // update this yourself as the four handlers above fire
  long id_   = 0;                       // id of the order currently in flight (0 = none)
  long seq_  = 0;                       // bump this yourself to mint each new id_
  long left_ = 0;                       // remaining unfilled quantity of the order in flight
};

// ------------------------------------------------------------------- strategy

// Strategies only express intent. No ids, no acks, no "am I allowed to send".
class JoinBidStrategy {
 public:
  JoinBidStrategy(OrderManager& oms, long size) : oms_(oms), size_(size) {}

  void onBook(long bid, long ask) { (void)bid; (void)ask; /* TODO: bid size_ at bid */ }
  void onPositionLimitHit()       { /* TODO: pull the quote */ }

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
  {  // requote three times while the first order is still pending
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
  {  // re-quote after an ack, then a stale ack arrives
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
