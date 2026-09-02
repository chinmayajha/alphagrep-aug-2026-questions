# Problem

The exchange allows you at most **5 orders per second**. Going over gets you flagged.

Write a class with a method that is called **before** each order is sent, which says
whether that order may be sent. Count how many were flagged.

```
RateLimiter:
  RateLimiter(maxOrders, windowMs)
  allow(nowMs)        // true = send it, false = flag and drop it
  flagged()
```

Orders are **not** evenly spaced. One arrives at 10ms, the next at 90ms, the next at 91ms.
You get the current time as a millisecond timestamp.

## What should happen

Limit 5 per 1000ms:

1. Orders at 0, 10, 20, 30, 40 — all allowed.
2. Order at 50 — flagged. Five have already gone in the last second.
3. Order at 1000 — allowed. The order at t=0 has left the window.
4. Orders at 995, 996, 997, 998, 999 all allowed, then orders at 1000, 1001 — flagged.
   A counter that resets every second would wrongly allow those. That's the trap.
5. What's the memory footprint if the limit is 5? What if it's 50,000?
