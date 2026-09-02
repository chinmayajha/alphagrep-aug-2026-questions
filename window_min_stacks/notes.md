# Answer

Two tricks stacked on top of each other.

**A stack that knows its own minimum.** When you push, also store the min of everything at
or below that entry: `min(newValue, currentTop.min)`. Then `min()` is a read of the top,
and `pop()` needs no fixing up because each entry carries its own answer. O(1) everything.

**A queue built from two stacks.** New prices go into the `in` stack. To remove the oldest,
if `out` is empty, pop everything from `in` and push it into `out` — that reversal puts the
oldest price on top of `out`. Then pop `out`.

Put them together: the window is that queue, and

```
window min = min(in.min(), out.min())
```

skipping whichever stack is empty. That's it.

## Why it's O(1) amortised

Each price is pushed to `in` once, moved to `out` at most once, and popped once. Three
operations per price over its whole life, so O(1) per tick averaged, even though one tick
can move the whole window. Say "amortised" out loud — a single tick is O(k) worst case, and
if they ask about worst-case-per-tick latency, that's a real answer: a monotonic deque or
splitting the drain across ticks.

## Why a single stack can't do it

A window needs insertion at the new end and removal at the old end — that's queue
semantics, and a stack only gives you one end. The two-stack queue is the standard way to
get a FIFO out of LIFO primitives, and the min-carrying entries survive the reversal
because each entry's min is computed from the stack it currently sits in.

The normal answer to sliding-window-min is a monotonic deque, which they have deliberately
banned. If you're asked why the deque works and the stack doesn't: the deque evicts from
both ends (expired at the front, dominated at the back), which is exactly the capability a
stack lacks. Mention it, then do it with stacks.

## If they poke

- *Window by time, not count* ("min over the last 60 seconds"): store `(timestamp, price)`
  and evict from `out` while its top is too old. The structure doesn't change. This is the
  version you'd actually have in a trading system, since ticks don't arrive on a metronome.
- *Fewer than k prices so far:* define it — min over what you have, or undefined until the
  window fills. Ask.
- *Duplicates:* nothing special, `<=` vs `<` doesn't matter for a min.
- *Min and max at once:* carry both in each entry.
- *Rolling sum or mean:* carry a running sum per stack the same way. Rolling **median** is
  the interesting one — stacks won't do it, you need two heaps.
- *Memory:* 2k entries of (value, min). If k is huge and prices are 8 bytes, mention that
  the deque version stores fewer.
- *Overflow:* prices as integer ticks in `long`, not floats.

## Similar questions in this family

Min stack, queue from two stacks, and sliding-window max are all standard, and this
question is those three welded together — the USACO sliding-window material lists the
two-stack construction as the alternative to the monotonic deque, and the same sliding
window minimum has been reported in quant researcher interviews elsewhere. Adjacent ones
worth a pass: stock span, next greater element, largest rectangle in a histogram, max sum
subarray of size k, and "design a structure with O(1) push/pop/min".
