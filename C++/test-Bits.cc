#include <stdio.h>
#include <iostream>

using namespace std;

class A {
public:
  A() {}
  ~A() {}
  virtual void do2()=0;

  static int _sI;
  int mI;
};

int A::_sI = 100;

class B : public A {
public:
};

int main() {
    char mystring[] = "I am Jackie";
    printf("%s\n", &mystring[5]);

    cout << "sizeof A = " << sizeof(A) << endl;
    cout << "sizeof B = " << sizeof(B) << endl;

    cout << "sizeof int = " << sizeof(int) << endl;
    cout << "sizeof int* = " << sizeof(int*) << endl;
}
