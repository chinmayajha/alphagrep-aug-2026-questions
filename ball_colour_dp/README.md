# Problem

You have an array of length N. Each cell independently holds 0, 1 or 2, each with
probability 1/3.

Each number does something to a ball passing through it:

- **0** paints the ball white
- **1** paints the ball black
- **2** flips its colour

You put a **white** ball in at the start. It passes through cell 0, then cell 1, and so on
to the end.

**What is the probability that the ball coming out is white?**

Write a recurrence first, then turn it into code. Then handle the general case where cell
`i` holds 0, 1, 2 with probabilities `(a_i, b_i, c_i)` instead of a uniform third each.

```
whiteProb(n)                 // uniform case
whiteProb(cells)             // per-cell probabilities
```

## What should happen

| N | answer |
|---|---|
| 0 | 1 |
| 1 | 1/3 |
| 2 | 5/9 |

Then:

1. What happens to the answer as N grows? Can you see it without computing?
2. Your recursion for the general case branches three ways per cell. What's the cost, and
   what does memoising buy you?
3. N is 10^9. Now what?
