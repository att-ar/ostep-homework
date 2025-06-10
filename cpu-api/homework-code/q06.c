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

    /* Useful if you fork more than once but the main process has a specific prereq */
    int child = waitpid(rc, NULL, 0);
    printf("PID of completed child [%d]\n", child);
    return 0;
}
