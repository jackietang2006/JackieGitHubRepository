#include <iostream>
#include <vector>
#include <string>
#include <iterator>
 
using namespace std;

class A {
public:
    A() { cout << "A()" << endl; }
    A(const A&) { cout << "A(const A&)" << endl; }
    ~A() { cout << "~A()" << endl; }

    const A& operator=(const A&a) {
        _a = a._a;
        cout << "Memberwise assignment" << endl;
        return *this;
    }
int _a;
};

int main()
{
    A a;
    A b = a;
    vector<A> mVector;
    mVector.reserve(100);
    cout<<"Vector Capacity = " << mVector.capacity() << endl;
    cout<<"Vector Size = " << mVector.size() << endl;
    mVector.push_back(A());
    mVector.push_back(A());
    mVector.push_back(A());
    mVector.push_back(A());
    mVector.push_back(A());
    cout<<"after push: Vector Capacity = " << mVector.capacity() << endl;
    cout<<"after push: Vector Size = " << mVector.size() << endl;
 
    vector<A> mVec2 = mVector;
    cout<<"after copy: Vector Capacity = " << mVector.capacity() << endl;
    cout<<"after copy: Vector Size = " << mVector.size() << endl;
    mVec2 = mVector;
    cout<<"after assign: Vector Capacity = " << mVector.capacity() << endl;
    cout<<"after assign: Vector Size = " << mVector.size() << endl;
    return 0;
}
