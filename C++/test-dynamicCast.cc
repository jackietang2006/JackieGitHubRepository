#include <stdio.h>
#include <iostream>
#include <typeinfo>
#include <pthread.h>
using namespace std;

class Base {
public:
    Base() : baseInt(100) {
      cout << "Constructor: Base()" << endl;
    }
    virtual ~Base() {
      cout << "Destructor: ~Base()" << endl;
    }

    void doit(int a = 1){
      cout << "Base:: doit()" << endl;
    }
    int baseInt;
};

class Derived : public Base {
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

};

int main() {
    Base* dp = new Derived;
    //Base* dp = new Base;

    printf("[%x]: Main-Thread: typeid(dp) = %s\n", pthread_self(), typeid(*dp).name());
    cout << "dp = " << dp << " typeif = " << typeid(*dp).name() << endl;
    printf("dp = %p\n", dp);

/*
    try {
        Derived &dp2 = dynamic_cast<Derived&>(*dp);
        cout << "dp2 = " << &dp2 << endl;
        dp2.doit();
    } catch (bad_cast &bc) {
        printf("[%x]: Catch-bad_cast\n", pthread_self());
        cout << "Catch-bad_cast: " << bc.what() << endl;
        cout << "Catch-bad_cast: dp = " << dp << endl;
    }
     catch (...) {
        printf("[%x]: Catch-All\n", pthread_self());
        cout << "Catch-All: dp = " << dp << endl;
    }
    delete dp;
*/

}
