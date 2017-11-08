#include <stdio.h>
#include <iostream>
#include <typeinfo>
#include <pthread.h>
using namespace std;

class Base {
public:
    explicit Base(int a=100) : baseInt(a) {
      cout << "Constructor: Base() baseInt = " << baseInt << endl;
    }
    virtual ~Base() {
      cout << "Destructor: ~Base()" << endl;
    }

    int baseInt;
    int baseInt2;
    static int baseInt3;
};

int Base::baseInt3;

int main() {
    //Base *dp = new Base;
    Base dp;
    Base dp2(dp);

    cout << "dp.baseInt = " << dp.baseInt << " dp.baseInt2 = " << dp.baseInt2 << " dp.baseInt3 = " << dp.baseInt3 << endl;
    cout << "dp2.baseInt = " << dp2.baseInt << " dp2.baseInt2 = " << dp2.baseInt2 << " dp2.baseInt3 = " << dp2.baseInt3 << endl;
}
