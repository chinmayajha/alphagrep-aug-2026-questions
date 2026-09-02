// Skeleton. Fill in the TODOs until main() passes.
// Build: g++ -std=c++17 -o template template.cpp && ./template

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

// TODO: design and implement this. Stacks only -- see problem.md.
class WindowMin {
 public:
  explicit WindowMin(size_t k) : k_(k) {}

  void onPrice(long px) { (void)px; /* TODO */ }

  long min() const { return 0; /* TODO */ }

  size_t size() const { return 0; /* TODO */ }
  bool   full() const { return size() == k_; }

 private:
  size_t k_;
  // TODO: your storage goes here.
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
