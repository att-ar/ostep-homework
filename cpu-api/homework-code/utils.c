#include "utils.h"

void wait_or_die()
{
    int wc = wait(NULL);
    assert(wc > 0);
}

int fork_or_die()
{
    int rc = fork();
    assert(rc >= 0);
    return rc;
}

int open_or_die(const char *path, int flags, int mode)
{
    int fd = open(path, flags, mode);
    assert(fd != -1);
    return fd;
}

void write_or_die(int fd, const void *buf, size_t len)
{
    ssize_t res = write(fd, buf, len);
    assert(res != -1);
}
