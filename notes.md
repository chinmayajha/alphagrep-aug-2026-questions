# Answer

Keep two things: **what the trader wants** and **what the exchange has**. Four states for
the order:

```
None  --send-->  PendingNew  --ack-->  Live  --cancel-->  PendingCancel
  ^                                                             |
  +------------- cancel accepted / rejected / filled -----------+
```

After *every* event, run the same function:

- `None` → if the trader wants a quote, send it.
- `Live` → if what the exchange has isn't what the trader wants, cancel it.
- `PendingNew` / `PendingCancel` → do nothing. You're not allowed to send anything.

That's the whole design.

## The two ideas that matter

**Keep only the latest quote, don't queue them.** Ten quote updates while an order is
pending produce one cancel and one new order at the tenth price. If you queue the updates
you'll send stale prices and burn ten round trips.

**"Cancel before the ack" needs nothing extra.** In `PendingNew` you do nothing, because
you can't send anything anyway. When the ack arrives, the state becomes `Live`, the same
function runs again, sees a mismatch, and cancels then. You don't have to remember "a
cancel was requested" — the comparison between wanted and held already tells you.

**One id per order, and ignore messages for any other id.** One line at the top of each
handler. That's what makes late and duplicate acks harmless instead of a special case.

## If they poke

- *Same quote re-sent?* Do nothing. Cancel/replace would lose queue position for free.
- *Send rejected?* Drop the quote, otherwise you resend it forever. (Retry only for a
  transient reject like a rate limit.)
- *Cancel rejected?* Two reasons: the order is already filled (it's gone, move on), or it's
  still resting (retry, but cap the retries so you can't loop).
- *Fill arrives?* Reduce the remaining quantity; when it hits zero the order is gone. Book
  the fill even if it's for an order you already gave up on — position has to be right.
- *No ack ever arrives?* Timer per send. On expiry stop quoting and escalate. Never assume
  a timed-out order is dead; that's how you end up with two live orders.
- *Price change with a modify/amend message?* Then you don't need cancel-then-new, and an
  amend-down can keep queue priority.
- *Threads?* One event loop, one thread, quotes and acks on the same queue. Don't put a
  lock around this state machine.
