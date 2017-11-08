#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

void
goinside(int arg)
{
    printf("Inside: arg = %d\n", arg);

    sleep(5);
  
    if (arg == 0) {
    }
    else {
      char arr[1024000];
      goinside (arg-1);
    }
}

int
main(int argc, char *argv[])
{
    int opt = -1;
    int stacksize = -1;
    while ((opt = getopt(argc, argv, "s:")) != -1) {
        printf("%s [-s stacksize]\n", argv[0]);
        sleep(10);
        switch (opt) {
        case 's':
            stacksize = strtoul(optarg, NULL, 0);
            break;

        default:
            fprintf(stderr, "Usage: %s [-s stack-size]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (stacksize == -1) {
        fprintf(stderr, "Usage: %s [-s stacksize]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    goinside(stacksize);
    exit(EXIT_SUCCESS);
}
