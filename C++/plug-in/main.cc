#include <stdio.h>

#ifdef __cplusplus
extern "C" {
void myFunc();
}
#endif

void myStaticFunc() {
#ifdef __cplusplus
    printf("calling myStaticFunc()\n");
#endif
}

int main() {
    myStaticFunc();

    myFunc();
}
