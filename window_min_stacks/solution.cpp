// Solution. Build: g++ -std=c++17 -o solution solution.cpp && ./solution

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

// A stack that knows its own minimum. Each entry carries the min of everything at
// or below it, so min() is just a look at the top.
class MinStack {
 public:
  void push(long v) {
    s_.push_back({v, s_.empty() ? v : std::min(v, s_.back().second)});
  }
  long pop() {
    long v = s_.back().first;
    s_.pop_back();
    return v;
  }
  long   min()   const { return s_.back().second; }
  bool   empty() const { return s_.empty(); }
  size_t size()  const { return s_.size(); }

 private:
  // vector as storage, but only ever push_back / pop_back / back: it is a stack.
  std::vector<std::pair<long, long>> s_;
};

// A queue built from two stacks, where both stacks know their min. The window min
// is then just the smaller of the two stack minimums.
class WindowMin {
 public:
  explicit WindowMin(size_t k) : k_(k) {}

  void onPrice(long px) {
    in_.push(px);
    if (size() > k_) evictOldest();
  }

  long min() const {
    if (in_.empty())  return out_.min();
    if (out_.empty()) return in_.min();
    return std::min(in_.min(), out_.min());
  }

  size_t size() const { return in_.size() + out_.size(); }
  bool   full() const { return size() == k_; }

 private:
  void evictOldest() {
    // Draining in_ into out_ reverses the order, so out_'s top is the oldest price.
    if (out_.empty())
      while (!in_.empty()) out_.push(in_.pop());
    out_.pop();
  }

  MinStack in_, out_;
  size_t   k_;
};

int main() {
  {
    WindowMin w(3);
    const long px[]  = {5, 2, 7, 1, 8, 3};
    const long want[] = {2, 1, 1, 1};
    int i = 0;
    for (long p : px) {
      w.onPrice(p);
      if (w.full()) {
        std::cout << "min = " << w.min() << "\n";
        assert(w.min() == want[i++]);
      }
    }
    assert(i == 4);
  }
  {  // rising prices: the min is always the element about to expire
    WindowMin w(2);
    w.onPrice(1);
    w.onPrice(2);
    assert(w.min() == 1);
    w.onPrice(3);
    assert(w.min() == 2);
    w.onPrice(4);
    assert(w.min() == 3);
  }
  {  // duplicates, and a partially filled window
    WindowMin w(3);
    w.onPrice(4);
    assert(w.min() == 4);
    w.onPrice(4);
    w.onPrice(9);
    assert(w.min() == 4);
    w.onPrice(9);
    assert(w.min() == 4);
    w.onPrice(9);
    assert(w.min() == 9);
  }
  std::cout << "ok\n";
}
