# AlphaGrep — August 2026 Interview Questions

Five self-contained C++17 practice problems, each in its own folder.

## Layout

Every folder follows the same shape:

| File | What it is |
|---|---|
| `README.md` | the problem statement |
| `template.cpp` | a skeleton with `TODO`s — fill it in |
| `solution.cpp` | a reference answer |
| `notes.md` | the answer explained, plus likely follow-ups |

Try each `template.cpp` against its own `README.md` before looking at `solution.cpp` or
`notes.md` — those two are written as an answer key, not a hint.

## Problems

| Folder | Topic |
|---|---|
| [`ball_colour_dp`](ball_colour_dp/README.md) | Probability recurrence: a ball repainted/flipped as it passes through N cells |
| [`connector_oop`](connector_oop/README.md) | Interface design: a polymorphic base class, virtual destructors, inheritance |
| [`ordermanagementsystem`](ordermanagementsystem/README.md) | Stateful design: keeping one order in sync with an exchange over async acks/fills |
| [`rate_limiter`](rate_limiter/README.md) | Sliding-window rate limiting, exact over arbitrary (not fixed-second) windows |
| [`window_min_stacks`](window_min_stacks/README.md) | Sliding-window minimum, using only stacks (no deque, no multiset) |

## Build and run

Each file builds standalone:

```bash
g++ -std=c++17 -o template template.cpp && ./template
```

Swap in `solution.cpp` the same way to check the reference answer runs clean.

## Editor setup

`compile_flags.txt` at the repo root tells clangd (or any compilation-database-aware
tool) to parse everything as C++17. Without it, standard-library headers gated on
`__cplusplus >= 201703L` (like `<optional>`) can silently vanish under a language-server's
older default and produce misleading diagnostics.
