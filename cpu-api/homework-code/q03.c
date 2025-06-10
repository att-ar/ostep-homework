#include <stdio.h>
#include "utils.h"

int main()
{
    int rc = fork_or_die();
    if (rc == 0)
    {
        printf("Hello\n");
        exit(0);
    }

    // ensure that child prints first
    usleep(50 * 1000);
    printf("Goodbye\n");

    return 0;
}
