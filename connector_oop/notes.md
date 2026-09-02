# Answer

The base class is an **interface**: pure virtual methods, a virtual destructor, no more
state than every connector genuinely shares.

```cpp
class Connector {
 public:
  virtual ~Connector() = default;
  Connector(const Connector&) = delete;             // a live socket isn't copyable
  Connector& operator=(const Connector&) = delete;

  virtual bool        connect()    = 0;
  virtual void        disconnect() = 0;
  virtual const char* name() const = 0;
  bool connected() const { return connected_; }
 protected:
  Connector() = default;
  bool connected_ = false;
};
```

And the function takes the base **by reference**:

```cpp
bool bringUp(Connector& c) { return c.connect(); }
```

Four things to say while you write it, because they're what's being marked:

**`= 0`, not a default body.** There is no sensible generic way to connect to an exchange,
so the base shouldn't pretend. That also makes `Connector` abstract, which means nobody can
accidentally instantiate a connector that connects to nothing.

**`virtual ~Connector()`.** You will hold these in a `vector<unique_ptr<Connector>>` and
destroy them through a base pointer. Without a virtual destructor only the base destructor
runs and the derived one is skipped — leaked socket, leaked buffer. The assert on
`g_dtors` in the file is checking exactly this.

**By reference or pointer, never by value.** Pass by value and you get object slicing: the
derived part is copied away and the base version is called. Here you can't even do it — the
class is abstract, so `bringUp(Connector c)` fails to compile, which is the strong form of
the same protection. If the base had a default `connect()` body, it would compile and
silently do the wrong thing. That contrast is worth stating.

**`override` on every override.** It's what turns a typo'd signature (`connect() const`)
from a silently-non-overriding new function into a compile error.

## If they poke

- *Forgot `virtual` on `connect()`?* `bringUp` calls the base version. Static type wins.
- *What's in the object?* A vtable pointer plus members. `sizeof(BinanceConnector)` is not
  `sizeof(int)`.
- *Cost of a virtual call?* An indirect call through the vtable: no inlining, a possible
  branch misprediction and an extra cache line. Fine for connecting; not fine on a per-tick
  hot path. In low-latency code you'd reach for templates/CRTP, `std::variant`, or a
  compile-time-selected connector so dispatch disappears. Saying this in a trading
  interview is worth more than any of the OOP trivia.
- *Can a constructor be virtual?* No. And calling a virtual from a constructor dispatches
  to the base version, because the derived part doesn't exist yet.
- *Can a pure virtual have a body?* Yes, callable via `Connector::connect()`, and common
  for pure virtual destructors.
- *Can you have a pointer or reference to an abstract class?* Yes — that's the whole
  mechanism. You just can't create an object of it.
- *Construction and destruction order?* Base constructed first, destroyed last.
- *Where does the object come from?* A factory returning `unique_ptr<Connector>` keyed on a
  venue name, so `main` doesn't name concrete types either.
- *Why does this design matter?* Testability. A `MockConnector` that fails on the third
  attempt lets you test reconnect logic with no network. That's usually the real reason the
  interface exists.
- *RAII: connect in the constructor?* Tempting, but a constructor can only fail by
  throwing, and network connects fail routinely. Two-phase (construct, then `connect()`) is
  the normal choice; be ready to defend either.

## Similar questions in this family

This is the standard C++ polymorphism cluster, asked with a trading skin on it: pure
virtual functions and what makes a class abstract, why abstract classes can't be
instantiated while pointers and references to them are fine, object slicing on pass-by-value
and assignment, virtual destructors and what leaks without one, overloading vs overriding,
and base/derived construction order.

AlphaGrep's reported C++ rounds sit right next to this: virtual classes, implementing
`shared_ptr`, `sizeof` questions, and in a more recent onsite, rvalue references, perfect
forwarding, variadic arguments, fold expressions, writing an iterator that returns rvalues,
compile-time template metaprogramming, and implementing `std::is_same`. If the connector
question comes up, expect the session to keep going in that direction — so know how you'd
replace this vtable with a template, and know why you might.
