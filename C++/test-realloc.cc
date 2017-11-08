/*
 *  * realloc.c -- Let's get resize previously allocated block and
 *   * free that up for reuse
 *    */
/* void *realloc(void *, size_t num_bytes) */ /* realloc resizes */
#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
    int *pi, i, num = 0;
    int *pil = 0;   /* pointer set to NULL */
 
    puts("\nLet's try to get block of memory for integers u want...\n");
    scanf("%d", &num);
 
    /* assigned to int * type */
    pi = (int*)malloc(num * sizeof(int));
 
    /* confirm if allocation is done */
    if (pi == NULL) {
            puts("Error: System short of memory!");
            exit(1);
    }
 
    puts("Good! Now, let's use dynamic memory...\n");
    printf("User type in %d integers one by one...\n", num);
    for (i = 0; i < num; i++)
            scanf("%d", (pi + i));
 
    puts("");
 
    printf("User, let's see what had u typed...\n");
    for (i = 0; i < num; i++)
            printf("%3d", *(pi + i));
 
    puts("");
 
    puts("Yeah, I want to resize the previously allocated chunk,\n"
            "specify another value for integers you want now...");
    scanf("%d", &num);
    /* reallocate block using realloc() function */
    printf("pil = %p pi = %p before calling realloc(pi, size)\n",
             pil, pi);
    pil = (int*)realloc(pi, num * sizeof(int));
    printf("pil = %p pi = %p after calling realloc(pi, size)\n",
             pil, pi);
 
    /* confirm if allocation is done */
    if (pil == NULL) {
            puts("Error: System short of memory!");
            exit(1);
    }
 
    printf("Let's see the contents of resized block of [%p]: %d integers.\n",
             pil, num);
    for (i = 0; i < num; i++)
           printf("%3d", *(pil + i));
 
    puts("");
 
    puts("Good! Now, let's use new chunk...\n");
    printf("User type in %d integers one by one...\n", num);
    for (i = 0; i < num; i++)
            scanf("%d", (pil + i));
 
    puts("");
 
    printf("User, let's see what had u typed...\n");
    for (i = 0; i < num; i++)
           printf("%3d", *(pil + i));
 
    puts("");
    puts("Let's return allocated memory to system for reuse.");
    free(pil);      /* remember don't double free up memory */
 
    puts("Thank you!\n");
    return 0;
}
