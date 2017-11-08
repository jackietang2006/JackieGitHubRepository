#include <stdlib.h>
#include <iostream>
using namespace std;

class Base {
protected:
    Base() : baseInt(100) {
      cout << "Constructor: Base()" << endl;
    }
    virtual ~Base() {
      cout << "Destructor: ~Base()" << endl;
    }

    virtual void doit(int a = 1){
      cout << "Base:: doit()" << endl;
    }
    int baseInt;
};

/*
 * Protected Inheritance is a Implementation inheritance: All-public members of the base class become protected
 *                                                        All-protected members of the base class remain protected
 * Private   Inheritance is a Implementation inheritance: All-public members of the base class become private
 */
class Derived : protected Base {
public:
    Derived() {
      cout << "Constructor: Derived() Base::baseInt = " << baseInt << endl;
    }
    ~Derived() {
      cout << "Destructor: ~Derived()" << endl;
    }
    void doit(int a = 1){
      Base::doit();
      cout << "Derived:: doit()" << endl;
    }
    virtual void dome(){
      cout << "Derived:: dome()" << endl;
    }

};

class Derived2 : public Derived {
public:
    Derived2() {
      cout << "Constructor: Derived2() Base::baseInt = " << baseInt << endl;
    }
    ~Derived2() {
      cout << "Destructor: ~Derived2()" << endl;
    }
    void doit(int a = 1){
      Base::doit();
      cout << "Derived2:: doit()" << endl;
    }
    void dome(){
      cout << "Derived2:: dome(int)" << endl;
    }

};

int main() {
    //Every ip[i] is an array - const
    int (*ip)[1024] = new int[4][1024];
    cout << "ip[0] = " << ip[0] << endl;
    cout << "ip[0][0] = " << ip[0][0] << endl;
    cout << "ip[0][1023] = " << ip[0][1023] << endl;
    cout << "ip[0][1024] = " << ip[0][1024] << endl;
    cout << "ip[1] = " << ip[1] << endl;
    cout << "ip[1][0] = " << ip[1][0] << endl;
    cout << "ip[1][1023] = " << ip[1][1023] << endl;
    cout << "ip[1][1024] = " << ip[1][1024] << endl;

    //int *ip2 = reinterpret_cast<int*>( malloc(1024*sizeof(int)) );
    int *ip2 = static_cast<int*>( malloc(1024*sizeof(int)) );

    //int **ip = new int*[4];
    delete []ip;

    Derived* dp = new Derived;
    dp->doit();
    delete dp;

    Derived* dp2 = new Derived2;
    dp2->doit();
    dp2->dome();
    delete dp2;

    Derived dp2Obj;
    dp2Obj.doit();
    dp2Obj.dome();

}
