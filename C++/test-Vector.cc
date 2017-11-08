#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <new>

using namespace std;

class Foo {
public:
  int val() { return _val; }
  Foo() : _val(0) {
      cout << "Foo()..." << endl;
  }
  Foo(const Foo&) : _val(0) {
      cout << "Foo(const Foo&)..." << endl;
  }
  ~Foo() { cout << "~Foo()..." << endl; }

private:
protected:
  int _val;
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr,"ERROR, no chunk provided\n");
    exit(1);
  }

  int chunk = atoi(argv[1]);

/*
  vector<Foo> fvec(4);
  cout << "fvec[4] has been created..." << endl;

  Foo farray[3];
  cout << "farray[3] has been created..." << endl;
*/
  vector<Foo> myvec;
  cout << "Size: " << myvec.size() << " Capacity: " << myvec.capacity() << endl;

  for ( int i = 0; i < chunk; i++)
  {
    myvec.push_back(Foo());
    cout << "Size: " << myvec.size() << " Capacity: " << myvec.capacity() << endl;
  }

  return 0;
}
