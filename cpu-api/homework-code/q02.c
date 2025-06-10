#include <stdio.h>
#include <fcntl.h>    // for O_* flags
#include <sys/stat.h> // for S_* modes
#include <string.h>
#include "utils.h"

/*
Writing to same file (race conditions)
Remember that on fork() children get access to all open file descriptors
- this is different from spawn() where exec() is used to clean the child process

It seems that `write()` is atomic for buffers smaller than a page (4KB normally).
So i can't actually visualize race conditions unless i artifically start and stop the write
with a loop which is unrealistic.
*/
int main()
{

    // open the file before forking
    const char filename[16] = "./q02.txt";
    const int fd = open_or_die(filename, O_CREAT | O_WRONLY, S_IRWXU);

    const int rc = fork_or_die();
    if (rc == 0)
    {
        // Process B: write (unbuffered)
        char buf[128];
        int len = snprintf(buf, sizeof(buf), "Writing from Process B [%d]\n", getpid());
        write_or_die(fd, buf, len);
        exit(0);
    }

    // Process A: write
    char buf[128];
    const int len = snprintf(buf, sizeof(buf), "Writing from Process A [%d], parent of [%d]\n", getpid(), rc);
    write_or_die(fd, buf, len);

    wait_or_die();
    close(fd);

    /*
    There's a memory leak since strdup() allocates memory that's never freed.
    But since execvp() replaces the entire process image,
    the leak doesn't matter in this case.
    */
    char *myargs[3];
    myargs[0] = strdup("cat");    // program: cat
    myargs[1] = strdup(filename); // argument: file to cat
    myargs[2] = NULL;             // marks end of array
    execvp(myargs[0], myargs);    // runs cat

    // unreachable as long as there's no error from the execvp
    return 0;
}
