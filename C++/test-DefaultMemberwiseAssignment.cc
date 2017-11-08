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
  Base() {
      cout << "Base()..." << endl;
  }
  Base(const Base&) {
      cout << "Default Memberwise Copy: Base(const Base&)..." << endl;
  }
  Base& operator=(const Base&) {
      cout << "Default Memberwise Assignment: operator=(const Base&)..." << endl;
  }
  ~Base() { cout << "~Base()..." << endl; }

private:
};

class Base2 {
friend class Foo;
public:
  Base2() {
      cout << "Base2()..." << endl;
  }
/*
  Base2(const Base2&) {
      cout << "Default Memberwise Copy: Base2(const Base2&)..." << endl;
  }
*/
  Base2& operator=(const Base2&) {
      cout << "Default Memberwise Assignment: operator=(const Base2&)..." << endl;
  }
  ~Base2() { cout << "~Base2()..." << endl; }

private:
};

class Foo {
public:
  Foo() : _base2(), _base() {
      cout << "Foo()..." << endl;
  }
  Foo(const Foo& f) : _base2(f._base2), _base(f._base) {
      cout << "Default Memberwise Copy: Foo(const Foo&)..." << endl;
  }
  Foo& operator=(const Foo& arg) {
      _base = arg._base;
      _base2 = arg._base2;
      cout << "Default Memberwise Assignment: operator=(const Foo&)..." << endl;
  }
  ~Foo() { cout << "~Foo()..." << endl; }

private:
  Base _base;
  Base2 _base2;
};

int main(int argc, char *argv[]) {
  //if (argc < 2) {
  //  fprintf(stderr,"ERROR, no chunk provided\n");
  //  exit(1);
  //}

  Foo foo1;
  Foo foo2;
  foo2 = foo1;

  return 0;
}
