/*
 * Overloaded functions are within the same class.
 * Overriding functions are cross class hierarchy
 * and both the function prototype & return type must match.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <new>

#define STRLEN 64

using namespace std;

class Base {
public:
  Base(const char* s) {
      memset(_string, 0, STRLEN);
      strncpy(_string, s, strlen(s));
      cout << "Base(const string&)..." << endl;
  }

  ~Base() { cout << "~Base()..." << endl; }

  void doit() {
      cout << "Base::doit()..." << endl;
  }

private:
  char _string[STRLEN];
};

class Base2 : public Base {
public:
  Base2(const char* s) : Base(s) {
      cout << "Base2(const string&)..." << endl;
  }

  ~Base2() { cout << "~Base2()..." << endl; }

  void doit() {
      cout << "Base2::doit()..." << endl;
  }
  void doit(int) {
      cout << "Base2::doit(int)..." << endl;
  }
};

int main(int argc, char* argv[]) {
    if (argc < 1) {
        fprintf(stderr,"ERROR, no compared strings provided\n");
        exit(1);
    }
    Base *bpbase = new Base("base");
    bpbase->doit();

    Base2 *bp = new Base2("b2");
    bp->doit();
    
}
