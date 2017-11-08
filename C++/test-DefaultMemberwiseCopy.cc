#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <new>

using namespace std;

class Base {
friend class Foo;
public:
  Base() : _mptr(NULL) {
      cout << "Base()..." << endl;
  }
  Base(void* ptr) : _mptr(ptr) {
      cout << "Base()..." << endl;
  }
  Base(const Base& b) : _mptr(b._mptr) {
      cout << "Default Memberwise Copy: Base(const Base&)..." << endl;
  }
  Base& operator=(const Base&b) {
      cout << "Default Memberwise Assignment: operator=(const Base&)..." << endl;
      _mptr = b._mptr;
  }
  ~Base() { cout << "~Base()..." << endl; }

  void* _mptr;

private:
};

class Base2 {
friend class Foo;
public:
  Base2() : _mptr(NULL) {
      cout << "Base2()..." << endl;
  }
  Base2(void* ptr) : _mptr(ptr) {
      cout << "Base2()..." << endl;
  }
  Base2(const Base2& b2) : _mptr(b2._mptr) {
      cout << "Default Memberwise Copy: Base2(const Base2&)..." << endl;
  }
  Base2& operator=(const Base2&b2) {
      cout << "Default Memberwise Assignment: operator=(const Base2&)..." << endl;
      _mptr = b2._mptr;
  }
  ~Base2() { cout << "~Base2()..." << endl; }

  void* _mptr;
private:
};

class Foo {
friend ostream& operator<<(ostream &os, const Foo& foo) {
  os << "_base._mptr = " << foo._base._mptr << " : _base2._mptr = " << foo._base2._mptr << endl;
  return os;
}
public:
  Foo(void* ptr) : _base2(ptr), _base(ptr) {
      cout << "Foo()..." << endl;
  }
  //Foo(const Foo& f) : _base2(f._base2), _base(f._base) { //Good: Only 1 function call of memberwisecopy constructor
  Foo(const Foo& f) { //Bad: 2 function calls of default constructor and memberwise assignment for all members
      cout << "Default Memberwise Copy: Foo(const Foo&)..." << endl;
      _base = f._base;
      _base2 = f._base2;
  }
  Foo& operator=(const Foo&f) {
      cout << "Default Memberwise Assignment: operator=(const Foo&)..." << endl;
      _base = f._base;
      _base2 = f._base2;
  }
  ~Foo() { cout << "~Foo()..." << endl; }

private:
  Base _base;
  Base2 _base2;
};

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr,"ERROR, no chunk provided\n");
    exit(1);
  }

  int chunk = atoi(argv[1]);

  Foo foo1((void*)(100));
  Foo foo2(foo1);

  cout << "foo1: " << foo1 << endl;
  cout << "foo2: " << foo2 << endl;

  return 0;
}
