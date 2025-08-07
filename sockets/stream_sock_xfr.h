/* Header file for the stream_sock_xfr(client/server) codes
 * xfr here in the filename means just the transferring of ASCII bytes from one program to another, as thats what the data between our processes are
 * for building a simple iterative server client setup that uses UNIX Domain Stream Sockets to communicate with each other
 * */

#include <sys/un.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define SV_SOCK_PATH "/tmp/stream_sock_xfr"

#define BUF_SIZE 100

