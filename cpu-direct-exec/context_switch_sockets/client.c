#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

const int NUM_ROUND_TRIPS = 100000;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: ./client.o <send port>\n");
        exit(1);
    }
    unsigned portno = atoi(argv[1]);
    int sockfd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Couldn't open socket");
        close(sockfd);
        exit(1);
    }

    // server address struct (to send to)
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));           // reset the memory
    serv_addr.sin_family = AF_INET;                     // ipv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // listen on localhost (127.0.0.1)
    serv_addr.sin_port = htons(portno);                 // convert to big-endian

    // timer start
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    char buf[128];
    int num_bytes;
    for (int i = 0; i < NUM_ROUND_TRIPS; i++)
    {
        // Send a message to the server
        snprintf(buf, sizeof(buf), "ping %d", i);
        num_bytes = sendto(sockfd, buf, strlen(buf), 0,
                           (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        if (num_bytes < 0)
        {
            perror("sendto failed");
            close(sockfd);
            exit(1);
        }
        // since we know the server is on the other end of this socket
        // and it is sending it back to this address
        // we can use recv instead of recvfrom
        num_bytes = recv(sockfd, buf, sizeof(buf) - 1, 0);
        if (num_bytes < 0)
        {
            perror("recv failed");
            close(sockfd);
            exit(1);
        }

        buf[num_bytes] = '\0'; // not needed but good practice
    }
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

    printf("PID [%d]: Final ping received from server echo \'%s\'\n", getpid(), buf);

    // close and exit
    close(sockfd);
    return 0;
}
