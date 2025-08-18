#include "iterative_udp_echo.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int sfd, j;
	size_t len;
	ssize_t numRead;
	char buf[BUF_SIZE];

	if (argc < 2 || strcmp(argv[2], "--help") == 0) {
		fprintf(stderr, "%s host msg...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* construct server address from the command line argument */
	sfd = inetConnect(argv[1], SERVICE, SOCK_DGRAM);
	if (sfd == -1) {
		perror("Could not connect to server socket");
		exit(EXIT_FAILURE);
	}

	/* send remaining command line arguments to server as separate datagrams */
	for (j = 2; j < argc; j++) {
		len = strlen(argv[j]);
		if (write(sfd, argv[j], len) != len)
			perror("partial/failed write");

		numRead = read(sfd, buf, BUF_SIZE);
		if (numRead == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}

		printf("[%ld bytes] %.*s\n", (long) numRead, (int)numRead, buf);
	}
	exit(EXIT_SUCCESS);
}
