// Solution. Build: g++ -std=c++17 -o solution solution.cpp && ./solution

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

// Exact "at most N in any window of W". Keep the timestamps of the last N accepted
// orders in a ring buffer. ts_[next_] is the oldest of those N: if it is still inside
// the window, accepting one more would put N+1 orders in the window.
class RateLimiter {
 public:
  RateLimiter(size_t maxOrders, long windowMs)
      : ts_(maxOrders, kNever), win_(windowMs) {}

  bool allow(long nowMs) {
    if (nowMs - ts_[next_] < win_) {
      ++flagged_;
      return false;
    }
    ts_[next_] = nowMs;
    next_ = (next_ + 1) % ts_.size();
    return true;
  }

  long flagged() const { return flagged_; }

 private:
  static constexpr long kNever = -1000000000;  // older than any real timestamp

  std::vector<long> ts_;
  long              win_;
  size_t            next_    = 0;
  long              flagged_ = 0;
};

int main() {
  {
    RateLimiter rl(5, 1000);
    for (long t : {0L, 10L, 20L, 30L, 40L}) assert(rl.allow(t));
    assert(!rl.allow(50));
    assert(!rl.allow(999));
    assert(rl.allow(1000));   // the order at t=0 has left the window
    assert(!rl.allow(1001));  // the one at t=10 has not
    assert(rl.allow(1010));
    assert(rl.flagged() == 3);
  }
  {  // the window-boundary trap: a per-second counter would allow all ten
    RateLimiter rl(5, 1000);
    for (long t : {995L, 996L, 997L, 998L, 999L}) assert(rl.allow(t));
    assert(!rl.allow(1000));
    assert(!rl.allow(1001));
  }
  {  // limit of one
    RateLimiter rl(1, 1000);
    assert(rl.allow(0));
    assert(!rl.allow(999));
    assert(rl.allow(1000));
  }
  std::cout << "ok\n";
}
