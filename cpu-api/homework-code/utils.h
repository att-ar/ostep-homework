#ifndef __utils_h__
#define __utils_h__

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>   // for fork(), write(), close()
#include <sys/wait.h> // for wait()
#include <fcntl.h>    // for open()

void wait_or_die(void);
int fork_or_die(void);
int open_or_die(const char *, int, int);
void write_or_die(int fd, const void *buf, size_t count);

#endif // __utils_h__
