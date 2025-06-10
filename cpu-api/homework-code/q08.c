#include <stdio.h>
#include "utils.h"

int main()
{
    // pipefd[0] is read-end
    // pipefd[1] is write-end
    int pipefd[2];
    assert(pipe(pipefd) == 0);

    // make child that will write
    int writeC = fork_or_die();
    if (writeC == 0)
    {
        // close read end
        close(pipefd[0]);

        // replace stdout with the write end of pipefd
        // dup2(oldfd, newfd) (closes newfd silently)
        dup2(pipefd[1], STDOUT_FILENO);

        // now write to the pipe
        printf("Writing this from Process B [%d]\n", getpid());
        exit(0);
    }

    // make child that will read
    int readC = fork_or_die();
    if (readC == 0)
    {
        // close write end
        close(pipefd[1]);

        // replace stdin with the read end of pipefd
        dup2(pipefd[0], STDIN_FILENO);

        // now read from stdin (it is actually the pipe)
        char buf[128];
        // this will error if the input is longer than 128 bytes
        assert(read(STDIN_FILENO, buf, 128) >= 0);

        // now print a wrapper sentence to prove it worked
        printf("Process C [%d] caught Process B saying: \n\t%s\n", getpid(), buf);

        exit(0);
    }

    // wait for reading to finish
    waitpid(readC, NULL, 0);
    return 0;
}
