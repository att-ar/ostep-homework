#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int sockfd;
void handle_sigint(int sig)
{
    close(sockfd);
    exit(0); // exit the process
}

/*
Going to use UDP so that each round trip is 2 context switches
syn and ack packets with TCP would cause way more context switches and I don't really want
to run `dtruss` and count

Total time for 100000 iterations: 3439491000 nanoseconds.
Average time per round trip: 34394.910000 nanoseconds.
Estimated Average time per switch: 17197.455000 nanoseconds.
PID [30866]: Final ping received from server echo 'ping 99999'
*/

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: ./server.o <listen port>\n");
        exit(1);
    }
    unsigned portno = atoi(argv[1]);

    // AF_INET for internet addresses (ipv4)
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Couldn't open socket");
        exit(1);
    }

    // server address struct, internet style
    struct sockaddr_in serv_addr;

    // reset server address struct
    // bzero((char*)&serv_addr, sizeof(serv_addr));
    // Idk why there's a char* typecast ^
    // I think this one makes more intuitive sense:
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;                     // ipv4
    serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // listen on localhost (127.0.0.1)
    // convert to big-endian (conform to network byte order)
    serv_addr.sin_port = htons(portno);

    // UDP doesn't require listen() after bind()
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Could not bind server");
        close(sockfd);
        exit(1);
    }

    // register ctrl+c handler
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    // The sigemptyset function initializes a signal set to exclude all signals,
    //  effectively creating an empty signal set.
    sigemptyset(&sa.sa_mask);
    // Do not set SA_RESTART (prevents interrupted syscalls from restarting)
    //  this way the recvfrom won't just continue to block after the ctrl+c
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) < 0)
    {
        perror("Failed to register signal handler");
        close(sockfd);
        exit(1);
    }

    /*
    // if I wasn't using loopback, and needed to print the IP addr
    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof(ipstr));
    printf("PID [%d]: Listening on %s:%d\n", getpid(), ipstr, portno);
    */
    printf("PID [%d]: Server listening on port %d. Press Ctrl+C to stop.\n", getpid(), portno);

    // this struct will get filled in with the client's address info
    struct sockaddr_in cli_addr;
    char buf[128];
    int num_bytes;
    while (1)
    {
        socklen_t cli_len = sizeof(cli_addr);
        // recv and recvfrom can be replaced by read() since sockfd is a file descriptor
        //  should be blocking since I didn't change the fd settings with fcntl
        // recvfrom will populate the address of the message sender into `cli_addr`
        // makes it very easy to just sendto() back to the sender
        int num_bytes = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&cli_addr, &cli_len);
        if (num_bytes < 0)
        {
            perror("recvfrom failed");
            exit(1); // in a multithreaded scenario, would kill a thread instead of the running program
        }
        // not actually needed since we are trimming it off in the sendto():
        buf[num_bytes] = '\0';

        // Echo back to sender for benchmarking purposes
        num_bytes = sendto(sockfd, (const char *)buf, num_bytes, 0, (struct sockaddr *)&cli_addr, cli_len);
        if (num_bytes < 0)
        {
            perror("sendto failed");
            exit(1); // in a multithreaded scenario, would kill a thread instead of the running program
        }
    }

    return 0;
}
