#include <syslog.h>
#include "iterative_udp_echo.h"
#include "../daemons/become_daemon.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int sfd;
	ssize_t numRead;
	socklen_t len;
	struct sockaddr_storage claddr;
	char buf[BUF_SIZE];
	char addrStr[IS_ADDR_STR_LEN];

	if (becomeDaemon(0) == -1){
		perror("becomeDaemon");
		exit(EXIT_FAILURE);
	}

	sfd = inetBind(SERVICE, SOCK_DGRAM, NULL);
	if (sfd == -1) {
		syslog(LOG_ERR, "Could not create server socket (%s)", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* receive datagrams from clients and return them to the client */

	for (;;) {
		len = sizeof(struct sockaddr_storage);
		numRead = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr *) &claddr, &len);
		if (numRead == -1) {
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}

		if (sendto(sfd,buf, numRead, 0, (struct sockaddr *) &claddr, len) != numRead) {
			syslog(LOG_WARNING, "Error echoing response to %s (%s)", inetAddressStr((struct sockaddr *) &claddr, len, addrStr, IS_ADDR_STR_LEN), strerror(errno));
		} 
	}
}
