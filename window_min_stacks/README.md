# Problem

A stock price arrives once every second.

At any point, report the **minimum price over the last k prices**.

**Constraint: you may only use stacks.** No deque, no monotonic deque, no multiset, no
sorting, no scanning the window on every tick.

```
WindowMin:
  WindowMin(k)
  onPrice(price)      // called once per second
  min()               // minimum over the current window
```

Aim for O(1) amortised per tick.

## What should happen

Prices 5, 2, 7, 1, 8, 3 with k = 3:

| window | min |
|---|---|
| 5 2 7 | 2 |
| 2 7 1 | 1 |
| 7 1 8 | 1 |
| 1 8 3 | 1 |

Then think about:

1. Prices only go up: 1, 2, 3, 4. The minimum is always the oldest price — the one about
   to leave. Does your structure handle that without rescanning?
2. Fewer than k prices so far. What does `min()` mean?
3. Duplicate prices in the window.
