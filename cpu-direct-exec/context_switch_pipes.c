#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // for fork(), write(), close()
#include <sys/wait.h> // for wait()
#include <assert.h>
#include <time.h>
// Mach API for macOS thread affinity
#include <mach/mach.h>
#include <mach/thread_policy.h>
#include <mach/mach_error.h>

const int NUM_ROUND_TRIPS = 1000000; // Repeat context switch 2 * 1M times
const int USE_THREAD_TAG = 1;

/**
 * @brief Sets the thread affinity tag for the calling thread on macOS.
 * Threads with the same tag are suggested to run on the same core.
 */
void set_thread_affinity(int tag)
{
    // Get the Mach thread port for the current thread.
    thread_t thread = mach_thread_self();

    // Define the affinity policy structure.
    thread_affinity_policy_data_t policy;
    policy.affinity_tag = tag;

    // Set the policy for the thread. This tells the macOS scheduler to try
    // and run this thread on the same core as other threads with the same tag.
    kern_return_t kr = thread_policy_set(
        thread,
        THREAD_AFFINITY_POLICY,
        (thread_policy_t)&policy,
        THREAD_AFFINITY_POLICY_COUNT);

    if (kr != KERN_SUCCESS)
    {
        fprintf(stderr, "Failed to set thread affinity for PID %d: %s\n",
                getpid(), mach_error_string(kr));
    }

    // Release the thread port.
    mach_port_deallocate(mach_task_self(), thread);
}

/**
 * @brief Handles reading from a file descriptor with error checking.
 * @param in The file descriptor to read from.
 * @param buf The buffer to store the read data.
 * @param size The number of bytes to read.
 * @param iter The current iteration number for logging.
 */
void handle_read(const int in, const char *buf, const int size, const int iter)
{
    int numBytes = read(in, (void *)buf, size); // buf can be cast to (void*)buf
    if (numBytes < 0)
    {
        perror("read failed");
        fprintf(stderr, "(fd %d) Read failed at iter %d", in, iter);
        // close before exiting
        close(in);
        exit(1);
    }
}

/**
 * @brief Handles writing to a file descriptor with error checking.
 * @param out The file descriptor to write to.
 * @param buf The buffer containing data to write.
 * @param size The number of bytes to write.
 * @param iter The current iteration number for logging.
 */
void handle_write(const int out, const char *buf, const int size, const int iter)
{
    int numBytes = write(out, (void *)buf, size); // buf can be cast to (void*)buf
    if (numBytes < 0)
    {
        perror("write failed");
        fprintf(stderr, "(fd %d) Write failed at iter %d", out, iter);
        // close before exiting
        close(out);
        exit(1);
    }
}

/**
 * @brief The first worker process. It loops, reading from one pipe and
 * writing to another, forcing context switches.
 */
void read_write(const int repeat, const int in, const int out)
{
    char buf[64];
    printf("Read-Write PID [%d]\n", getpid());

    for (int i = 0; i < repeat; i++)
    {
        handle_read(in, buf, 64, i);   // read from output of write-read
        handle_write(out, buf, 64, i); // write to input of write-read
    }
    // there will be a dangling write but wtv
    printf("Printing from Read-Write: %s", buf);
}

/**
 * @brief The second worker process. It writes an initial message and then
 * loops, writing and reading to force context switches.
 */
void write_read(const int repeat, const int in, const int out)
{
    char buf[64];
    int len_would_write = snprintf(buf, sizeof(buf), "Hey there! - [%d]\n", getpid());
    if (len_would_write >= sizeof(buf))
    {
        fprintf(stderr, "Write message is too long\n");
        exit(0);
    }

    printf("Write-Read PID [%d]\n", getpid());

    for (int i = 0; i < repeat; i++)
    {
        handle_write(out, buf, 64, i); // write to input of read-write
        handle_read(in, buf, 64, i);   // read from output of read-write
    }
}

/*
Total time for 1000000 iterations: 3347350000 nanoseconds.
Average time per round trip: 3347.350000 nanoseconds.
Estimated Average time per switch: 1673.675000 nanoseconds.
*/
int main()
{
    // try and keep both processes on the same core.
    // forked processes inherit the affinity namespace of parent
    // NOTE: on my machine I am not allowed to use this
    //     "(os/kern) service not supported"
    set_thread_affinity(USE_THREAD_TAG);

    int pipefd1[2];
    assert(pipe(pipefd1) == 0);
    int pipefd2[2];
    assert(pipe(pipefd2) == 0);

    const int write_read_in = pipefd1[0];
    const int write_read_out = pipefd2[1];

    const int read_write_in = pipefd2[0];
    const int read_write_out = pipefd1[1];

    // timer start
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    // fork both workers
    int write_read_worker = fork();
    if (write_read_worker == 0)
    {
        // unused pipes
        close(read_write_in);
        close(read_write_out);

        write_read(NUM_ROUND_TRIPS, write_read_in, write_read_out);
        // used pipes
        close(write_read_in);
        close(write_read_out);

        exit(0);
    }

    int read_write_worker = fork();
    if (read_write_worker == 0)
    {
        // unused pipes
        close(write_read_in);
        close(write_read_out);

        read_write(NUM_ROUND_TRIPS, read_write_in, read_write_out);
        // used pipes
        close(read_write_in);
        close(read_write_out);

        exit(0);
    }

    // unused pipes
    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    // wait
    int wc1 = waitpid(write_read_worker, NULL, 0);
    int wc2 = waitpid(read_write_worker, NULL, 0);

    // timer end
    clock_gettime(CLOCK_MONOTONIC, &end);

    long total_elapsed_ns;
    double avg_elapsed_ns;
    double avg_elapsed_switch_ns;

    total_elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000L +
                       (end.tv_nsec - start.tv_nsec);
    avg_elapsed_ns = (double)total_elapsed_ns / NUM_ROUND_TRIPS;
    avg_elapsed_switch_ns = avg_elapsed_ns / 2.0;

    printf("\nTotal time for %d iterations: %ld nanoseconds.\n", NUM_ROUND_TRIPS, total_elapsed_ns);
    printf("Average time per round trip: %f nanoseconds.\n", avg_elapsed_ns);
    printf("Estimated Average time per switch: %f nanoseconds.\n", avg_elapsed_switch_ns);

    return 0;
}
