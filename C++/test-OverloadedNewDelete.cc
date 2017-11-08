#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <new>
#include <cstddef>

using namespace std;

class Screen {
public:
  void *operator new( size_t );
  void *operator new[]( size_t );
  void operator delete ( void*, size_t );
  void operator delete[] ( void*, size_t );

  Screen() { cout << "Screen()\n"; }
  virtual ~Screen() { cout << "~Screen()..." << endl; }

private:
  Screen *next;
  static Screen *freeStore;
  static const int screenChunk;
};

Screen* Screen::freeStore = NULL;
const int Screen::screenChunk = 24;

void* Screen::operator new( size_t size ) {
  cout << "Calling overloaded operator new(size_t): size = " << size << endl;
  Screen *p;

  if ( !freeStore ) {
    size_t chunk = screenChunk * size;
    //freeStore = p = reinterpret_cast< Screen* > ( new char[ chunk ] );
    //freeStore = p = ( Screen* ) ( new char[ chunk ] );
    //Error at compiling: freeStore = p = dynamic_cast< Screen* > ( new char[ chunk ] );
    freeStore = p = static_cast< Screen* > ( malloc( chunk ) );
    
    for(; p != &freeStore[ screenChunk-1 ]; p++)
    {
        p->next = p + 1;
    }
    p->next = 0;
  }

  p = freeStore;
  freeStore = freeStore->next;
  return p;
}

void* Screen::operator new[]( size_t size ) {
  cout << "Calling overloaded operator new[](size_t): size = " << size << endl;
  Screen *p;

  if ( !freeStore ) {
    size_t chunk = screenChunk * size;
    //freeStore = p = reinterpret_cast< Screen* > ( new char[ chunk ] );
    freeStore = p = ( Screen* ) ( new char[ chunk ] );
    
    for(; p != &freeStore[ screenChunk-1 ]; p++)
    {
        p->next = p + 1;
    }
    p->next = 0;
  }

  p = freeStore;
  freeStore = freeStore->next;
  return p;
}

void Screen::operator delete( void *ptr, size_t size ) {
  cout << "Calling overloaded operator delete(void*, size_t): " << ptr << " size = " << size << endl;
  ( static_cast< Screen* >( ptr ) )->next = freeStore;
  freeStore = static_cast< Screen* >( ptr );
}

void Screen::operator delete[]( void *ptr, size_t size ) {
  cout << "Calling overloaded operator delete[](void*, size_t): " << ptr << " size = " << size << endl;
  ( static_cast< Screen* >( ptr ) )->next = freeStore;
  freeStore = static_cast< Screen* >( ptr );
}

int main(int argc, char* argv[]) {
/*
    if (argc < 3) {
        fprintf(stderr,"ERROR, no compared strings provided\n");
        exit(1);
    }
*/
    
    cout << "sizeof(Screen) = " << sizeof(Screen) << endl;
    Screen *p = new Screen[10];
    cout << "New Screen: p = " << p << endl;

    // delete NULL ptr does nothing, neither is the destructor nor the overloaded
    // operator delete invoked.
    //p = NULL;
    delete []p;
    cout << "Delete Screen: p = " << p << endl;

    Screen *cp = new Screen;
    cout << "New Screen: cp = " << cp << endl;
    cout << "sizeof(void*) = " << sizeof(void*) << endl;
    delete cp;
    cout << "Delete Screen: cp = " << cp << endl;
}
