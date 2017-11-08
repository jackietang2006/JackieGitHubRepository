#include <iostream>
#include <new>

using namespace std;

const int chunk = 16;

class Foo {
public:
  int val() { return _val; }
  Foo() : _val(0) {}
  ~Foo() { cout << "~Foo..." << endl; }

private:
protected:
  int _val;
};

char* buf = new char[sizeof(Foo) * chunk];

int main() {
  // Create a Foo object in buf;
  Foo *pb = new(buf) Foo;

  if (pb->val() == 0)
    cout << "PlacementNew worked" << endl;

  // Can not release here!
  delete pb;

  // Rather - do Explicit Destructor Invocation
  pb->~Foo();
  return 0;
}
