/** The steps employed in the server for seqNum are:
    1. Initialise the server's sequence number either to 0 or the number provided in the command line argument
    2. Ignore the SIGPIPE signal, as this prevents the server from receiving the SIGPIPE signal if it tries to write to a socket whose peer has been closed,
    so instead the write() fails with EPIPE error
    3. Call getaddrinfo() to obtain a set of socket address structures for a TCP socket that uses the port unmber PORT_NUM
    4. Enter a loop that iterates through the previously obtained socket address structures. The loop terminates when it finds an socket address structure that can be used to 
    create and bind a socket
    5. Set the SO_REUSEADDR option for the socket created in previous step
    6. Mark the socket as a listening socket
    7. Commence the infinite loop that serviuces client iteratively.
    8. For each client, the server does the following steps
        a) Accept a new connection. The server passes NON NULL pointers to accept() call, in order to receive the address of the client.
        b) The server displays this client's address/IP plus port num on std out
        c) Read the client's message, which consists of new line terminated strings and reads how many sequence numbers does the client wants. The server converts this string
        to an integer and stores it in variable reqLen
        d) Send the current value of sequenceNumber to the client, encoding it as a newline terminated string. 
        The client can thus assume it has been allocated all the sequence numbers in the range of (seqNum -> seqNum + reqLen - 1)
        e) Update the value of server's sequence number by adding reqLen to its seqNum
 */


#define _BSD_SOURCE     /* to get definitions of NI_MAXHOST and NI_MAXSERV from <netdb.h> */

#include <netdb.h>
#include "net_stream_sock_seqnum.h"

#define BACKLOG 50

int main(int argc, char *argv[])
{
    uint32_t seqNum; 
    char reqLenStr[INT_LEN];    /* Length of the requested sequence */
    char seqNumStr[INT_LEN];    /* Start of the granted sequence */
    struct sockaddr_storage claddr;
    int lfd, cfd, optval, reqLen;
    socklen_t addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    #define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)

    char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s [init-seq-num]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    seqNum = (argc > 1) ? atoi(argv[1]) : 0;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    /* call getaddrinfo to obtain a list of addresses that we can try binding to */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;    /* unspecified, so allows both IPv4 and IPv6 */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;   /* Wildcard IP Address, service name is numeric */

    if (getaddrinfo(NULL, PORT_NUM, &hints, &result) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    /* walk through returned list structures until we find an address structure that corresponds correctly for binding and using a socket */

    optval = 1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1)
            continue;   // if errored, move on to the next address

        /* Set the socket option to be SO_REUSEADDR */
        if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            // binding was successful, we can break from the loop
            break;
        }

        // binding failed, move on and try the next address
        close(lfd);
    }

    if (rp == NULL) {
        perror("FATAL: could not bind to any address at all");
        exit(EXIT_FAILURE);
    }

    if (listen(lfd, BACKLOG) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);   // to release the allocated linked list of socket address structures from the getaddrinfo call

    /* iterative loop to listen and process client requests */
    for (;;) {

        addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(lfd, (struct sockaddr *) &claddr, &addrlen);
        if (cfd == -1) {
            fprintf(stderr, "accept");
            continue;
        }

        if (getnameinfo((struct sockaddr *) &claddr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
            snprintf(addrStr, ADDRSTRLEN, "(%s %s)", host, service);
        else
            snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");

        printf("Connection from %s\n", addrStr);

        /* Read client request, send sequence number back */
        if (readLine(cfd, reqLenStr, INT_LEN) <= 0) {
            close(cfd);
            continue;   /* failed read, skip request */
        }

        reqLen = atoi(reqLenStr);
        if (reqLen < 0) {     /* watch for misbehaving clients */
            close(cfd);
            continue;
        }

        snprintf(seqNumStr, INT_LEN, "%d\n", seqNum);   /* for sending the client the current seqNum as a newline terminated string, and client can assume the range per its ask */
        if (write(cfd, seqNumStr, strlen(seqNumStr)) != strlen(seqNumStr)) {
            fprintf(stderr, "Error on write");
        }

        seqNum += reqLen;   // update the server's sequence number
        if (close(cfd) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
    }
}
