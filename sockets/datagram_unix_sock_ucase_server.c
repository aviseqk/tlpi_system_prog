/* A simple UNIX Domain Datagram Socket Server
 *
 * */

#include "datagram_unix_sock_ucase.h"

int main(int argc, char *argv[])
{
    struct sockaddr_un svaddr, claddr;
    int sfd, j;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];
    
    /* create the server socket */
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket");
        exit(1);
    }

    /* bind the dsocket to a well-known address */
    if (strlen(SERVER_SOCK_PATH) > sizeof(svaddr.sun_path) - 1) {
        fprintf(stderr, "Server socket path too long: %s\n", SERVER_SOCK_PATH);
        exit(1);
    }

    if (remove(SERVER_SOCK_PATH) == -1 && errno != ENOENT)  {
        fprintf(stderr, "remove-%s", SERVER_SOCK_PATH);
        exit(1);
    }

    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SERVER_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    if (bind(sfd, (struct sockaddr *) &svaddr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(1);
    }

    /* receive messages, convert to uppercase and return to the client */
    for (;;) {
        len = sizeof(struct sockaddr_un);
        numBytes = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &claddr, &len);

        if (numBytes == -1) {
            perror("recvfrom");
            exit(1);
        }
        printf("Server received %ld bytes from %s\n", (long)numBytes, claddr.sun_path);

        for (j=0; j < numBytes; j++) {
            buf[j] = toupper((unsigned char)buf[j]);
        }

        if (sendto(sfd, buf, numBytes, 0, (struct sockaddr *) &claddr, len) != numBytes) {
            perror("sendto - partial write");
            exit(1);
        }
    }
}

