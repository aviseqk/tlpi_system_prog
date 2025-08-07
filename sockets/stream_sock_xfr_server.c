/* A Simple UNIX Domain Socket Server */
/* This program does the following
 * 1.
 * 2.
 * */

#include "stream_sock_xfr.h"
#define BACKLOG 5

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int sfd, cfd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1){
        perror("socket");
        exit(1);
    }

    /* construct the server socket address, bind a socket to it, and make it a listening socket */
    if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1) {     /* SAFETY CHECK - Make sure the socket path doesnt exceed the sizeof(addr.sun_path) at all */
        fprintf(stderr, "Server socket path too long: %s", SV_SOCK_PATH);
        exit(1);
    }

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT) {    /* SAFETY CHECK - to remove an existing socket if any at that pathname */
        fprintf(stderr, "remove: %s", SV_SOCK_PATH);
        exit(1);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));   /* clean or memset to 0, for all the data bytes of the struct */
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

    /* bind the socket to this socket path */
    if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sfd, BACKLOG) == 1) {
        perror("listen");
        exit(1);
    }

    /* handle all the client connections iteratively */
    for (;;) {
        /* accept a new connection, the connection is returned on a new socket, cfd in this case */
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1) {
            perror("accept");
            exit(1);
        }

        /* transfer data from connected socket to stdout until EOF is read */
        while((numRead = read(cfd, buf, BUF_SIZE)) > 0) {
            if (write(STDOUT_FILENO, buf, numRead) != numRead) {
                perror("partial/failed write");
                exit(1);
            }
        }

        if (numRead == -1) {
            perror("read");
            exit(1);
        }

        if (close(cfd) == -1) {
            perror("close");
            exit(1);
        }
    }
}
