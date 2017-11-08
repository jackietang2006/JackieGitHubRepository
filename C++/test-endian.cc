#include <stdio.h>

int main() {
    int a = 0x1234567;

    char* ca = (char*)&a;

    while(*ca)
      printf("%.2x", *ca++);

    printf("\n");

    //unsigned char arr[2] = {0x01, 0x00};
    unsigned char arr[2] = {1, 0};
    unsigned short us = *(unsigned short*)arr;
    printf("[10] = %d\n", us);

    printf("'0' = %d\n", '0');
    printf("'1' = %d\n", '1');

    unsigned char arr2[2] = {'1', '0'};
    unsigned short us2 = *(unsigned short*)arr2;
    printf("['1''2'] = %d\n", us2);
    printf("4948 = %d\n", 49 + 48*256);

    unsigned short us3 = 1<<8;
    
    printf("1<<8 = %d\n", us3);
    if (us3 == 0) {
        printf("I am Big-Endian\n");
    }
    else if(us3 == 256) {
    }
        printf("I am Little-Endian\n");
}
