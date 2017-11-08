#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short (*FP)(int len);

unsigned short factorialFP(int N) {
    if (N < 1) return 1;
    return (N*factorialFP(N-1));
}

unsigned short sumFP(int N) {
    unsigned short sum = 0;
    for(int i = N; i > 0; i--)
        sum += i;
    return sum;
}

unsigned short testFP(FP fp, int len) {
    //return ((*fp)(len));
    return (fp(len));
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr,"Usage - %s Method N\n", argv[0]);
        exit(1);
      }

    int N = atoi(argv[2]);
    if (!strcmp(argv[1], "Factorial")) {
        printf("Factorial of %d = %d\n", N, testFP(factorialFP, N));
    }
    else if (!strcmp(argv[1], "Sum")) {
        printf("Sum of %d = %d\n", N, testFP(sumFP, N));
    }
    else {
        fprintf(stderr,"Usage - %s Factorial|Sum N\n", argv[0]);
        exit(1);
    }
    return 0;
}
