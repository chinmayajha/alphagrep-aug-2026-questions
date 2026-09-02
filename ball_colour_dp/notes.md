# Answer

**Find the state.** The ball carries nothing but its current colour. Where it's been
doesn't matter. So one number is the entire state: `p(i) = P(white after i cells)`.

**Write the recurrence.** Cell `i+1` is 0, 1 or 2 with probability 1/3 each:

```
p(0)   = 1
p(i+1) = 1/3 * 1        (painted white)
       + 1/3 * 0        (painted black)
       + 1/3 * (1 - p(i))   (flipped, so white now iff it was black)
       = (2 - p(i)) / 3
```

Sanity check: `p(1) = 1/3`, which is right — after one cell the ball is white only if that
cell was a 0. `p(2) = 5/9`.

**Then the code writes itself.** Recursion with a memo array is one line. But the
recurrence has one dimension and each value depends only on the previous one, so you don't
need the array at all — a single rolling `double` in a loop. Say that; interviewers like
watching someone go memo → O(1) space.

**Then solve it.** Look for the fixed point: `p = (2-p)/3` gives `p = 1/2`. Subtract it
out and the recurrence becomes `p(n) - 1/2 = (-1/3)(p(n-1) - 1/2)`, so

```
p(n) = 1/2 + 1/2 * (-1/3)^n
```

The answer converges to **1/2** geometrically, alternating above and below. By n = 20 it's
1/2 to eleven decimal places. That's the satisfying part of the question, and it's the bit
that answers "what if N is a billion".

The 1/2 is not a coincidence: 0 and 1 are symmetric, and a flip preserves that symmetry, so
the only thing keeping the answer away from 1/2 is the memory of having started white — and
each cell destroys a third of what's left of it.

## Why memoisation matters here

For the uniform version it barely does — the chain is linear. It matters when you write the
recursion as "branch on the three possible cell values", because that's 3^n paths for an
answer that only depends on n. Recognising that all paths with the same `(index, colour)`
collapse into one state is the actual DP insight. State space is `n x 2`, and since colour
is complementary you can fold it into one number per index.

## If they poke

- *Per-cell probabilities* (cell i is 0/1/2 with probability a/b/c): same shape,
  `p = a + c*(1 - p)`. No closed form in general. This is the version where the DP framing
  earns its keep.
- *N = 10^9:* closed form, or note the general case is a product of 2x2 matrices and use
  fast exponentiation if all cells are identical.
- *Matrix view:* the step is a 2x2 stochastic matrix; its eigenvalues are 1 and -1/3, which
  is exactly where the `(-1/3)^n` came from. Bringing this up unprompted is a good signal.
- *Expected number of flips the ball undergoes:* n/3, by linearity.
- *Ball starts black, or starts white with probability q:* only `p(0)` changes; the
  recurrence is identical. Good check that you built the right abstraction.
- *Numerical care:* the deviation from 1/2 shrinks by 3x per cell, so it underflows fast in
  `double`. Fine here, worth mentioning.
- *Prove your answer:* simulate it. `simulate()` in the file is the habit they want to see —
  derive, then check against a Monte Carlo.

## Similar questions in this family

The technique — name the state, write a recurrence, memoise, then look for a closed form —
is the single most-tested pattern in quant interviews. The classics it shows up in:
expected flips until HH (and why HT is 4 while HH is 6), gambler's ruin and `p = k/N`,
random walk hitting times, expected number of uniforms until the sum exceeds 1, dice
optimal-stopping, absorbing Markov chains, and probability that pattern A appears before
pattern B. AlphaGrep's own reported research-intern rounds have included conditional
probability, Markov chains, mean/median and Sharpe ratio questions, so this exact style is
in scope.
