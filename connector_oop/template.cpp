// Skeleton. Fill in the TODOs until main() passes.
// Build: g++ -std=c++17 -o template template.cpp && ./template
//
// This one does not compile until you have written the interface and made the two
// connectors inherit from it. That is the exercise. main() is already correct.

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

int g_dtors = 0;

// TODO: this is the class you were given, empty. Make it an interface.
class Connector {
 public:
 protected:
  bool connected_ = false;
};

class BinanceConnector /* TODO: inherit */ {
 public:
  ~BinanceConnector() { ++g_dtors; }

  // TODO: connect(), disconnect(), name()

 private:
  int streamId_ = 7;   // state the base class knows nothing about
};

class OtherExchangeConnector /* TODO: inherit */ {
 public:
  ~OtherExchangeConnector() { ++g_dtors; }

  // TODO: same three methods, different behaviour

 private:
  long seqNum_ = 0;
};

// TODO: one function that works for every connector. Think hard about the parameter
// type before you write it, then try the other options and see what breaks.
bool bringUp(/* TODO */) { return false; }

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
  assert(g_dtors == 1);

  std::cout << "ok\n";
}
