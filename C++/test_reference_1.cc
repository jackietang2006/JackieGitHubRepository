#include <iostream>

int main() {
    const int ival =1024;

    const int *const & piref = &ival;

    int i = 0;
    const int& ri2 = 0;
}
