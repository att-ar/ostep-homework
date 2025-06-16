#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>

const int NUM_ITERATIONS = 25000000; // Repeat syscall 25M times

/*
Total time for 50000000 iterations: 18 502 638 000 nanoseconds.
Average time per task: 370.052760 nanoseconds.

That's a pretty long time just to trap into kernel space
    and then return-from-trap back
*/
int main()
{
    struct timespec start, end;
    long total_elapsed_ns;
    double avg_elapsed_ns;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_ITERATIONS; ++i)
    {
        /*
        A buffer size of 1 is fine since we're not actually reading any data,
        but you could also use NULL as the buffer when doing 0-byte reads:
        */
        read(STDIN_FILENO, NULL, 0); // 0-byte read
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    total_elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L +
                       (end.tv_nsec - start.tv_nsec);

    avg_elapsed_ns = (double)total_elapsed_ns / NUM_ITERATIONS;

    printf("Total time for %d iterations: %ld nanoseconds.\n", NUM_ITERATIONS, total_elapsed_ns);
    printf("Average time per task: %f nanoseconds.\n", avg_elapsed_ns);

    return 0;
}
