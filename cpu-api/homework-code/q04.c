#include <stdio.h>
#include <string.h>
#include "utils.h"

int main()
{

    int rc = fork_or_die();
    if (rc == 0)
    {
        // execl() takes a path and string args (not in an array)
        printf("execl()\n");
        assert(execl("/bin/ls", strdup("$HOME/Coding/ostep-homework/cpu-api/homework-code"), NULL) != -1);
        exit(0);
    }

    usleep(50 * 1000);
    rc = fork_or_die();
    if (rc == 0)
    {
        // execl() takes a filename or path and string args (not in an array)
        printf("\nexeclp()\n");
        assert(execlp("ls", strdup("$HOME/Coding/ostep-homework/cpu-api/homework-code"), NULL) != -1);
        exit(0);
    }

    usleep(50 * 1000);
    rc = fork_or_die();
    if (rc == 0)
    {
        // execl() takes a filename or path and string args (not in an array)
        printf("\nexeclp()\n");
        assert(execlp("ls", strdup("$HOME/Coding/ostep-homework/cpu-api/homework-code"), NULL) != -1);
        exit(0);
    }

    usleep(50 * 1000);
    rc = fork_or_die();
    if (rc == 0)
    {
        // execve() (execvpe is not available on macos)
        printf("\nexecve()\n");
        // 1. Define the custom environment variables for the child process
        char *custom_env[5] = {
            "DB_HOST=my_database_server",
            "API_KEY=your_secret_api_key_123",
            "LOG_LEVEL=DEBUG",
            "PATH=/usr/local/bin:/usr/bin:/bin", // Custom PATH for the child
            NULL                                 // envp array must be NULL-terminated
        };

        // 2. Define the arguments for the program to be executed
        char *args[4] = {
            "/bin/sh",
            "-c",
            "echo API_KEY=$API_KEY and LOG_LEVEL=$LOG_LEVEL",
            NULL // argv array must be NULL-terminated
        };

        assert(execve(args[0], args, custom_env) != -1);
    }

    return 0;
}
