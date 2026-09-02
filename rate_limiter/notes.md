# Answer

The rule is "at most N orders in **any** window of W milliseconds". So the only thing you
need to know before sending is: *when did the N-th most recent accepted order go out?* If
that's still inside the window, you'd be making it N+1. Flag it.

So keep the timestamps of the last N accepted orders. You never need more than N of them,
which means a ring buffer of N longs, and `next_` points at the oldest:

```
if (now - ts[next] < window)  -> flag, return false
ts[next] = now; next = (next + 1) % N; return true
```

O(1) time, O(N) memory, no allocation, no scanning. For N = 5 that's 40 bytes.

## The trap

The tempting version is a counter that resets every second. It's wrong: five orders at
995-999ms and five more at 1000-1004ms is ten orders in a 10ms span, and a per-second
counter allows all of them. Fixed windows let you burst up to twice the limit across a
boundary. Say this out loud even if you go on to implement the counter — knowing the
failure mode is most of the point.

The sliding-window **log** (a queue of timestamps, drop the ones older than `now - W`) is
the same algorithm as the ring buffer. The ring buffer just bounds the memory by
construction, since a log longer than N entries can never change an answer.

## Token bucket, and when it's the right answer instead

A token bucket refills at a fixed rate and lets you spend saved-up tokens as a burst. It's
the right model when the venue's constraint is a *rate* you must not exceed on average, or
when you want to smooth your own outflow. It's the wrong model here, because "5 per second"
as a hard count is exactly what the ring buffer answers and the bucket only approximates.
Know both and know which question each one answers.

## If they poke

- *Where does `now` come from?* A monotonic clock, sampled once per order and passed in.
  Passing the time in makes the class testable — that's why `allow(nowMs)` takes a
  parameter instead of calling the clock itself.
- *Query vs commit:* `allow()` consumes a slot as a side effect. If the send can fail after
  the check, you want `canSend()` and `recordSent()` separately, or you'll leak slots.
- *Per symbol, per session, per account?* A map from key to limiter. Then ask which limit
  the exchange actually enforces, because you have to count what they count.
- *Do cancels count?* At most venues yes, and often modifies too. Ask.
- *Multiple limits at once* (10/sec and 100/min): one limiter per rule, allow only if all
  agree. Check the tightest first.
- *Threads:* one gateway thread owning the limiter is the clean answer. If it must be
  shared, a mutex, and mention that the check-then-send has to be atomic or two threads
  both pass at N-1.
- *What do you do when flagged?* In trading you drop or replace the order, you don't queue
  it — a queued quote is a stale quote by the time it goes out. This ties straight back to
  the order-management question: on throttle, re-derive the newest desired state rather
  than sending the backlog.

## Similar questions in this family

Rate limiter design is a stock system-design question, so expect the standard menu:
fixed window counter, sliding window log, sliding window counter, token bucket, leaky
bucket, and their memory/accuracy trade-offs. The trading-flavoured variants: order
throttle per exchange session, message budget shared between orders and cancels,
self-trade-prevention counters, and "flag the client that breaches the limit" (the
detection version rather than the blocking version).
