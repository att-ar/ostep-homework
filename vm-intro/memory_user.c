#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

const int MAX_SIZE_MB = 500;
const int BYTES_PER_KB = 1024;

// Global vars
volatile bool running = true;
int *array = NULL;
long num_elements = 0;

// signal handler (ctrl-c) stops the thread
void handle_signal(int _signum)
{
    running = false;
}

void *array_loop(void *arg)
{
    long i = 0;
    while (running)
    {
        array[i] += 1;
        array[i] -= 1;
        i = (i + 1) % num_elements;
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "usage: %s <size> in MBs [<duration>] in seconds\n", argv[0]);
        exit(1);
    }
    int size = atoi(argv[1]);
    long duration = 0;
    if (argc == 3)
        // string to convert, pointer to point to 1st char after converted part, base for conversion
        duration = strtol(argv[2], NULL, 10);

    if (size > MAX_SIZE_MB)
    {
        // not giving more than 500 MBs
        fprintf(stderr, "size %d must not exceed 500\n", size);
        exit(1);
    }
    num_elements = (size * BYTES_PER_KB * BYTES_PER_KB) / sizeof(int);

    // update the global array to point to the new memory
    // Using calloc instead of malloc (we know we want contiguous memory for an array)
    array = (int *)calloc(num_elements, sizeof(int));
    if (array == NULL)
    {
        fprintf(stderr, "Failed to allocate array\n");
        free(array);
        exit(1);
    }

    // Register signal handler to toggle `running`
    signal(SIGINT, handle_signal);

    // Create and start thread
    pthread_t thread;
    if (pthread_create(&thread, NULL, array_loop, NULL) != 0)
    {
        fprintf(stderr, "Failed to create thread\n");
        free(array);
        exit(1);
    }

    // print process id
    printf("PID [%d]\n", (int)getpid());
    fflush(stdout); // flush the output buffer

    if (duration > 0)
    {
        sleep(duration);
        running = false;
    }

    // Wait for thread to finish
    void *result;
    pthread_join(thread, &result); // result is NULL in this case

    // ALWAYS free
    free(array);

    return 0;
}
