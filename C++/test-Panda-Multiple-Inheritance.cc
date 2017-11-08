#include <iostream>

using namespace std;

class Endangered {
public:
    Endangered() { cout << "Constructor Endangered()..." << endl; }
    void highlight() {}
    void print(int) const {}
};

class Animal {
public:
    Animal() { cout << "Constructor Animal()..." << endl; }
    void highlight(int, int) {}
};

class Bear : public Animal {
public:
    Bear() { cout << "Constructor Bear()..." << endl; }
    void print(int) const {}
};

class Panda : public Bear, public Endangered {
public:
    void cuddle() const {}
};

int main() {
    Panda pan;
}
