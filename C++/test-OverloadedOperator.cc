#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <new>

#define STRLEN 64

using namespace std;

class Foo {
friend bool operator==(const Foo&, const Foo&);
public:
  Foo(const char* s) {
      memset(_string, 0, STRLEN);
      strncpy(_string, s, strlen(s));
      cout << "Foo(const string&)..." << endl;
  }

  //bool operator==(const Foo &rhs);

  ~Foo() { cout << "~Foo()..." << endl; }

private:
  char _string[STRLEN];
};

/*
bool Foo::operator==(const Foo &rhs) {
    cout << "Calling member overloaded operator==" << endl;
    if (strlen(_string) != strlen(rhs._string)) {
        return false;
    }
    else
    {
        return strcmp( _string, rhs._string ) ? false : true;
    }
}
*/

bool operator==(const Foo &lhs, const Foo &rhs) {
    cout << "Calling friend overloaded operator==" << endl;
    if (strlen(lhs._string) != strlen(rhs._string)) {
        return false;
    }
    else
    {
        return strcmp( lhs._string, rhs._string ) ? false : true;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr,"ERROR, no compared strings provided\n");
        exit(1);
    }
    
    Foo f1(argv[1]);
    Foo f2(argv[2]);

    cout << "Comparing: " << argv[1] << " and " << argv[2] << " : " << (f1==f2) << endl;
    cout << "Comparing: " << argv[1] << " and " << argv[2] << " : " << (f1==argv[2]) << endl;
    cout << "Comparing: " << argv[1] << " and " << argv[2] << " : " << (argv[1]==f2) << endl;
}
