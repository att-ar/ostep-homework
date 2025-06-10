#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> // for wait()
#include <stdio.h>

int main()
{
    int rc = fork();
    if (rc == 0)
    {
        // should be an invalid PID because there are no children
        int grandchild = wait(NULL);
        printf("PID of completed grandchild [%d]\n", grandchild);
        exit(0);
    }

    int child = wait(NULL);
    printf("PID of completed child [%d]\n", child);
    return 0;
}
