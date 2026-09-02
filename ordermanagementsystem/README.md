# Problem

You sit between a trader and an exchange, and you own the order management system.

The trader quotes a **price and a quantity**. Your job is to keep an order at the exchange
matching that quote.

The classes are given. You implement the methods.

**Given to you:**

```
Exchange:
  sendOrder(id, side, price, qty)     // you call these
  cancelOrder(id)
```

The exchange replies **asynchronously**:

- send acknowledgement: accepted or rejected
- cancel acknowledgement: accepted or rejected
- fills

**The rule: at most one order may be live at the exchange at a time.**

So while you are waiting for an acknowledgement you cannot send anything new — you don't
yet know whether the order you sent is live. You *are* allowed to send a cancel for an
order you haven't received the acknowledgement for.

**Implement:**

```
OrderManager:
  onQuote(side, price, qty)      // trader's latest quote, qty 0 means no quote
  onSendAck(id, accepted)
  onCancelAck(id, accepted)
  onFill(id, qty)
```

Then add a simple strategy on top that produces the quotes.

## What should happen

Work through these and make sure your code does the right thing:

1. Trader quotes 100. You send it. Ack comes back. Fine.
2. Trader quotes 100, then 101, then 102 **before the ack arrives**. What do you send, and
   when?
3. Trader re-sends the same quote you already have live. What do you send?
4. Trader pulls the quote while your order is unacknowledged.
5. Your cancel is rejected because the order just got filled.
6. An acknowledgement arrives for an order you already gave up on.
