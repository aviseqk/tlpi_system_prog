/* A simple UNIX Domain Datagram Socket Client
 *
 * */

#include "datagram_unix_sock_ucase.h"

int main(int argc, char *argv[])
{
    struct sockaddr_un claddr, svaddr;
    int sfd, j;
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s msg..\n", argv[0]);
        exit(1);
    }

    /* create a socket and bind to a unique pathname/address based on this client's PID */
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(1);
    }

    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, sizeof(claddr.sun_path), "/tmp/ud_ucase_cl.%ld", (long)getpid());

    if (bind(sfd, (struct sockaddr *) &claddr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(1);
    }

    /* construct address of the server */
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SERVER_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    /* send messages to server, and echo responses on stdout */
    for (j = 1; j < argc; j++) {
        msgLen = strlen(argv[j]);
        if (sendto(sfd, argv[j], msgLen, 0, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) != msgLen) {
            perror("sendto");
            exit(1);
        } 

        numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if (numBytes == -1) {
            perror("recvfrom");
            exit(1);
        }

        printf("Response %d: %.*s\n", j, (int)numBytes, resp);
    }
    remove(claddr.sun_path);
    exit(0);
}
