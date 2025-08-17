/* The steps employed in the client program for Network Socket Stream seqNum are:
 * The program accepts two arguments, the first one -> the name of host on which the server is running, and second one -> length of sequence desired by the client
 Steps are :
 1. Call getaddrinfo() to obtain the set of socket address structures suitable for connecting to the TCP server bound to the specific host
 2. Enter a loop to iterate through the addresses and find a socket it connect and bind to the server. the kernel specifies an ephemeral port for this socket
 3. Send an integer specifying the length of client's desired sequence, sent as a newline terminated string(think of this as the marshaling step)
 4. Read the sequence number sent back by the server and print to stdout
 * */

#include "net_stream_sock_seqnum.h"
#include <netdb.h>

int main(int argc, char *argv[])
{
    char *reqLenStr;    /* Requested length of the sequence */
    char seqNumStr[INT_LEN];    /* Start of the granted sequence */
    int cfd;
    ssize_t numRead;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s server-host [sequence-len]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* call getaddrinfo to get list of socket addresses into result */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;    /* unspecified, so allows both IPv4 and IPv6 */
    hints.ai_flags = AI_NUMERICSERV;

    if (getaddrinfo(argv[1], PORT_NUM, &hints, &result) != 0) {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        cfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (cfd == -1)
            continue;

        if (connect(cfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(cfd); /* connect failed: close this address and try the next one */
    }

    if (rp == NULL) {
        fprintf(stderr, "FATAL: Could not connect to the server at all at any address");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    /* send the requested sequence length, with a terminating newline */
    reqLenStr = (argc > 2) ? argv[2] : "1";
    if (write(cfd, reqLenStr, strlen(reqLenStr)) != strlen(reqLenStr)) {
        perror("Partial/failed write to the server (reqLenStr)");
        exit(EXIT_FAILURE);
    }
    if (write(cfd, "\n", 1) != 1) {
        perror("Partial/failed write to the server (newLine)");
        exit(EXIT_FAILURE);
    }

    /* read and display the sequence number rreceived from the server */
    numRead = readLine(cfd, seqNumStr, INT_LEN);
    if (numRead == -1) {
        perror("readLine");
        exit(EXIT_FAILURE);
    }
    if (numRead == 0) {
        perror("FATAL: Unexpected EOF from the server");
        exit(EXIT_FAILURE);
    }

    printf("Sequence Number (recvd from server): %s", seqNumStr);

    exit(EXIT_SUCCESS);
}
