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
    vector<A>::iterator iter = mVector.begin();
    mVector.erase(iter);
    cout<<"after erasing: Vector Capacity = " << mVector.capacity() << endl;
    cout<<"after erasing: Vector Size = " << mVector.size() << endl;

    vector<A> m2Vec;
    m2Vec.push_back(A());
    cout<<"before assign: Vector Capacity = " << mVector.capacity() << endl;
    cout<<"before assign: Vector Size = " << mVector.size() << endl;
    mVector = m2Vec;
    cout<<"after assign m2 to m: Vector Capacity = " << mVector.capacity() << endl;
    cout<<"after assign m2 to m: Vector Size = " << mVector.size() << endl;
 
    return 0;
}
