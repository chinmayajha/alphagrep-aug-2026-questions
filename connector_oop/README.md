# Problem

You are connecting to two exchanges. There is a `Connector` base class, unimplemented, and
two derived ones:

```
Connector                    // given to you, empty
  BinanceConnector
  OtherExchangeConnector
```

In `main`, you create a `BinanceConnector`. You pass that object into a function, and
inside that function `connect()` is called on it. The function must not know or care which
exchange it was handed.

Implement the base class, make the two connectors inherit from it, and write the function.

## What should happen

1. `bringUp(binance)` calls Binance's `connect()`, not the base's.
2. The same function works for the other connector, unchanged.
3. What is the parameter type of that function, and why? What happens if you take it by
   value?
4. You hold connectors in a `vector` and delete them through a base pointer. Which
   destructor runs?
5. Should `connect()` have a default implementation in the base, or none at all?
