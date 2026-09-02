// Skeleton. Fill in the TODOs until main() passes.
// Build: g++ -std=c++17 -o template template.cpp && ./template
//
// Derive the recurrence on paper before you write any code.
//
// simulate() below is given: use it to check whatever you derive.

#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

double recursive(int n, std::vector<double>& memo) {
  (void)n; (void)memo;
  return 0.0;  // TODO: recursion + memo
}

double bottomUp(int n) {
  (void)n;
  return 0.0;  // TODO: same recurrence, iteratively. How much memory do you need?
}

double closedForm(int n) {
  (void)n;
  return 0.0;  // TODO: solve the recurrence in closed form.
}

struct Cell {
  double white, black, flip;
};

double bottomUp(const std::vector<Cell>& cells) {
  (void)cells;
  return 0.0;  // TODO: same shape, per-cell probabilities
}

double simulate(int n, int trials, unsigned seed = 42) {
  std::mt19937 rng(seed);
  std::uniform_int_distribution<int> cell(0, 2);
  int white = 0;
  for (int t = 0; t < trials; ++t) {
    bool isWhite = true;
    for (int i = 0; i < n; ++i) {
      switch (cell(rng)) {
        case 0:  isWhite = true;     break;
        case 1:  isWhite = false;    break;
        default: isWhite = !isWhite; break;
      }
    }
    white += isWhite;
  }
  return static_cast<double>(white) / trials;
}

static bool near(double a, double b, double eps = 1e-12) { return std::fabs(a - b) < eps; }

int main() {
  std::vector<double> memo(64, -1.0);

  assert(near(bottomUp(0), 1.0));
  assert(near(bottomUp(1), 1.0 / 3.0));
  assert(near(bottomUp(2), 5.0 / 9.0));

  for (int n = 0; n < 40; ++n) {
    assert(near(recursive(n, memo), bottomUp(n), 1e-12));
    assert(near(closedForm(n),      bottomUp(n), 1e-12));
  }

  std::vector<Cell> uniform(7, Cell{1.0 / 3, 1.0 / 3, 1.0 / 3});
  assert(near(bottomUp(uniform), bottomUp(7), 1e-12));

  std::vector<Cell> flipOnly(5, Cell{0.0, 0.0, 1.0});  // five certain flips
  assert(near(bottomUp(flipOnly), 0.0));               // white -> black -> ... -> black

  const double sim = simulate(5, 200000);
  std::cout << "n=5  exact " << bottomUp(5) << "  simulated " << sim << "\n";
  assert(std::fabs(sim - bottomUp(5)) < 0.01);

  std::cout << "n=1  " << bottomUp(1) << "\nn=2  " << bottomUp(2)
            << "\nn=20 " << bottomUp(20) << "\nok\n";
}
