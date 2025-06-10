#include "utils.h"
#include <stdio.h>

// Q1: separate memory spaces per process
int main()
{
    // value that will be mutated by both processes
    // since they are in their own memory space
    // there will not be any race conditions
    int x = 99;

    // process a
    int rc = fork_or_die();
    if (rc == 0)
    {
        // process b
        printf("Process B before mutate: x = %d\n", x);
        x = 10001;
        printf("Process B: x = %d\n", x);
        exit(0);
    }

    x = 101;
    printf("Process A before wait: x = %d\n", x);
    wait_or_die();
    printf("Process A after wait: x = %d\n", x);

    return 0;
}
