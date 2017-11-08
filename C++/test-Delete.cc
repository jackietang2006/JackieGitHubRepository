#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <new>

using namespace std;

class STRING {
public:
/*
  STRING(int) {
      cout << "STRING()..." << endl;
  }
*/
  ~STRING() { cout << "~STRING()..." << endl; }
};

class Foo {
public:
  int val() { return _val; }
  Foo() : _val(0) {
      cout << "Foo()..." << endl;
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

  //Foo (*pb)[3] = new Foo[chunk][3];
  //delete []pb;

  Foo **pt1 = new Foo*[3];

  vector<string> svec(10);
  vector<string> *pvec1 = new vector<string>(10);
  vector<string> *pv1 = &svec;
  vector<string> *pv2 = pvec1;

  vector<string> **pvec2 = reinterpret_cast<vector<string>**>( new vector<string>[10] );
  vector<string> **pvec3 = (vector<string>**)( new vector<string>[10] );

  const Foo* fp = new Foo[chunk];
  delete []fp;

  const STRING* cp = new STRING[chunk];
  delete []cp;

  return 0;
}
