#include <iostream>
#include <memory>
#include <vector>

using namespace std;

class X
{
public:
    X() { cout << "X()" << endl; }
    ~X() { cout << "~X()" << endl; }
};

int main() {
  auto_ptr<X> aptr(new X);
  auto_ptr<X> aptr2(aptr);

  cout << "after copy constr" << endl;
}
