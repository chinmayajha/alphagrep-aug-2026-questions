// Skeleton. Fill in the TODOs until main() passes.
// Build: g++ -std=c++17 -o template template.cpp && ./template

#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

// Decide what you must remember to answer that exactly, and how much of it.
class RateLimiter {
 public:
  RateLimiter(size_t maxOrders, long windowMs)
      : ts_(maxOrders, kNever), win_(windowMs) {}

  bool allow(long nowMs) {
    (void)nowMs;
    return false;  // TODO
  }

  long flagged() const { return flagged_; }

 private:
  static constexpr long kNever = -1000000000;  // older than any real timestamp

  std::vector<long> ts_;              // fixed at maxOrders entries (see the ctor) -- don't resize it
  long              win_;              // window length in ms, from the ctor
  size_t            next_    = 0;      // an index into ts_; yours to manage inside allow()
  long              flagged_ = 0;      // count of allow() calls that returned false
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
