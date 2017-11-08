#include <iostream>
#include <map>
#include <string>
#include <string.h>
#include <iterator>
 
using namespace std;

class Base {
friend ostream& operator<< ( ostream &os, const Base &rhs ) {
    os << rhs._name;
    return os;
}
friend bool operator< ( const Base &lhs, const Base &rhs ) {
        if( strcmp(lhs._name, rhs._name) < 0 ) {
          return true;
        }
        else
          return false; 
}

public:
    Base(char* name) : _name(name) {
        cout << "Base(const char*)" << endl;
    }
    Base(const Base& rhs) : _name(rhs._name) {
        cout << "Base(const Base&)" << endl;
    }

    bool operator<= ( const Base &rhs ) const {
        if( strcmp(_name, rhs._name) <= 0 ) {
          return true;
        }
        else
          return false; 
    }
    ~Base() {
        cout << "~Base()" << endl;
    }
private:
    char* _name;

};

int main()
{
    std::map<Base, int> mapOfWords;
    // Inserting data in std::map
    mapOfWords.insert(std::make_pair(Base((char*)"earth"), 1));
    mapOfWords.insert(std::make_pair(Base((char*)"moon"), 2));
    mapOfWords[Base((char*)"sun")] = 3;
 
    std::map<Base, int>::iterator it = mapOfWords.begin();
    for( ; it != mapOfWords.end(); it++ )
        std::cout<<it->first<<" :: "<<it->second<<std::endl;
 
return 0;
}
