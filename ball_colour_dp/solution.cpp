// Solution. Build: g++ -std=c++17 -o solution solution.cpp && ./solution
//
// Let p(i) = P(ball is white after passing i cells).  Only the current colour matters,
// so one number is the whole state.
//
//   p(0)   = 1                                    (we insert a white ball)
//   p(i+1) = 1/3 * 1  +  1/3 * 0  +  1/3 * (1 - p(i))
//          = (2 - p(i)) / 3
//
// Fixed point: p = (2 - p)/3  =>  p = 1/2, and p(n) - 1/2 = (-1/3) * (p(n-1) - 1/2),
// so p(n) = 1/2 + 1/2 * (-1/3)^n. It converges to 1/2 very fast.

#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

double recursive(int n, std::vector<double>& memo) {
  if (n == 0) return 1.0;
  if (memo[n] >= 0.0) return memo[n];
  return memo[n] = (2.0 - recursive(n - 1, memo)) / 3.0;
}

double bottomUp(int n) {  // same recurrence, O(1) memory
  double p = 1.0;
  for (int i = 0; i < n; ++i) p = (2.0 - p) / 3.0;
  return p;
}

double closedForm(int n) { return 0.5 + 0.5 * std::pow(-1.0 / 3.0, n); }

struct Cell {
  double white, black, flip;
};

double bottomUp(const std::vector<Cell>& cells) {
  double p = 1.0;
  for (const Cell& c : cells) p = c.white + c.flip * (1.0 - p);  // c.black * 0 drops out
  return p;
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
