#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    void* current_program_break = 0;
    
    current_program_break = sbrk(0);    

    printf("current program break = %x\n", current_program_break);

    //current_program_break = sbrk(4);

    sleep(10);

    char* allocM = (char*)malloc(1);
    current_program_break = sbrk(0);
    printf("current program break(0) = %x after new(4096)\n", current_program_break);

    sleep(10);
}
