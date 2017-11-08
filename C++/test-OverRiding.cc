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
      cout << "Base(const char*&)..." << endl;
  }

  virtual ~Base() { cout << "~Base()..." << endl; }

  virtual int doit(int) { cout << "Base::doit()" << endl; }

private:
  char _string[STRLEN];
};

class Base2 : public Base {
public:
  Base2(const char* s) : Base(s) {
      cout << "Base2(const char*&)..." << endl;
  }

  ~Base2() { cout << "~Base2()..." << endl; }

  int doit(int) { cout << "Base2::doit()" << endl; }

private:
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr,"ERROR, no compared strings provided\n");
        exit(1);
    }
    
    Base *ptr1 = new Base2( argv[1] );
    Base *ptr2 = new Base2( argv[2] );

    ptr1->doit(1);

}
