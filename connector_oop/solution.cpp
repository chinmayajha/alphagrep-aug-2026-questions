// Solution. Build: g++ -std=c++17 -o solution solution.cpp && ./solution

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

int g_dtors = 0;

// The interface. Pure virtual methods, a virtual destructor, and no state that a
// caller could copy by accident.
class Connector {
 public:
  virtual ~Connector() = default;

  Connector(const Connector&)            = delete;   // a live socket is not copyable
  Connector& operator=(const Connector&) = delete;

  virtual bool        connect()    = 0;
  virtual void        disconnect() = 0;
  virtual const char* name() const = 0;

  bool connected() const { return connected_; }

 protected:
  Connector() = default;
  bool connected_ = false;
};

class BinanceConnector : public Connector {
 public:
  ~BinanceConnector() override { ++g_dtors; }

  bool connect() override {
    std::cout << "binance: websocket handshake\n";
    connected_ = true;
    return true;
  }
  void        disconnect() override { connected_ = false; }
  const char* name() const override { return "binance"; }

 private:
  int streamId_ = 7;   // state the base class knows nothing about
};

class OtherExchangeConnector : public Connector {
 public:
  ~OtherExchangeConnector() override { ++g_dtors; }

  bool connect() override {
    std::cout << "other: fix logon\n";
    connected_ = true;
    return true;
  }
  void        disconnect() override { connected_ = false; }
  const char* name() const override { return "other"; }

 private:
  long seqNum_ = 0;
};

// By reference to the base. The function has no idea which exchange it was handed;
// the vtable sorts that out at the call.
bool bringUp(Connector& c) {
  if (!c.connect()) {
    std::cout << c.name() << ": failed\n";
    return false;
  }
  return c.connected();
}

// bool bringUp(Connector c);   // does not compile: Connector is abstract. If the base
                                // were concrete this would compile and silently slice
                                // streamId_ / seqNum_ away, calling the base's connect().

int main() {
  BinanceConnector b;
  assert(bringUp(b));
  assert(b.connected());

  std::vector<std::unique_ptr<Connector>> all;
  all.push_back(std::make_unique<BinanceConnector>());
  all.push_back(std::make_unique<OtherExchangeConnector>());
  for (auto& c : all) assert(bringUp(*c));

  g_dtors = 0;
  {
    std::unique_ptr<Connector> c = std::make_unique<OtherExchangeConnector>();
  }
  assert(g_dtors == 1);   // true only because ~Connector is virtual

  std::cout << "ok\n";
}
