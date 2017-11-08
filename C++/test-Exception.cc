#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <string>
#include <typeinfo>

using namespace std;

class Base {
friend ostream& operator<<( ostream &os, const Base &rhs ) {
    os << rhs._info;
    return os;
}
public:
    Base(const char* info) : _info(info) {
      cout << "Constructor: Base() - _info: " << *this << endl;
    }
    virtual ~Base() {
      cout << "Destructor: ~Base() - _info: " << *this << endl;
    }
private:
    string _info;
};

class BaseException : public Base {
public:
    BaseException(const char* info) : Base(info) {
      cout << "Constructor: BaseException()" << endl;
    }
    virtual ~BaseException() {
      cout << "Destructor: ~BaseException()" << endl;
    }

    void doit(int a = 1){
      cout << "Base:: doit()" << endl;
    }
};

class Derived : public Base {
public:
    Derived(const char* arg) : Base( arg ) {
      cout << "Constructor: Derived(arg)" << endl;
    }
    ~Derived() {
      cout << "Destructor: ~Derived()" << endl;
    }
};

int main() {
    try {
        Derived obj("I am Jackie");
        cout << "Create obj[" << pthread_self() << "]: " << obj << endl;

        throw BaseException("I am throwing a BaseException!");
    }
    catch (BaseException &e) {
        cout << "Catching BaseException[" << pthread_self() << "]: " << e << endl;
    }
    catch (Base &e) {
        cout << "Catching Base[" << pthread_self() << "]: " << e;
    }
    catch (string &e) {
        cout << "Catching string[" << pthread_self() << "]: " << e.size() << " - " << e.c_str() << endl;
    }
    catch (...) {
        cout << "Catching All[" << pthread_self() << "]: " << endl;
    }
    cout << "Exitting[" << pthread_self() << "]: Now" << endl;
}
