/* A Simple UNIX Domain Socket Client */
/* This program does the following
 * 1.
 * 2.
 * */

#include "stream_sock_xfr.h"

int main(int argc, char *argv[])
{
	struct sockaddr_un addr;
	int sfd;
	ssize_t numRead;
	char buf[BUF_SIZE];


 	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    	if (sfd == -1){
        	perror("socket");
        	exit(1);
    	}

	/* construct the server socket address, and make the connection */
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path) - 1);

	if (connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) == -1) {
		perror("connect");
		exit(1);
	}

	/* copy stdin to the socket */
	while((numRead = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
		if (write(sfd, buf, numRead) != numRead) {
			perror("partial/failed read");
			exit(1);
		}
	}

	if (numRead == 1) {
		perror("read");
		exit(1);
	}

	exit(0);
}
