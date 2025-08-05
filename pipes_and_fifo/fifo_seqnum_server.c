/* The functions of this FIFO server are :-
 * 1. create the server's well known single FIFO, and open it for reading (must be created before any client is created)
 * 2. open the server's FIFO for writing this time. this is so that even if all clients close their write end, server doesn't sees EOF
 * 3. ignore the SIGPIPE signal, so if server attempts to write to a client FIFO which has no reader, so rather than sending SIGPIPE(that kilss the process), we sent EPIPE error
 * 4. enter a loop that reads and responds to each incoming request, so to send, it constructs the name of the client FIFO, opens that FIFO and writes it
 * 5. if server has issues in opening a constructed client's FIFO, it abandones that client request by erroring that request
 **/

#include <signal.h>
#include "fifo_seqnum.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int serverFd, clientFd, dummyFd;
	char clientFifo[CLIENT_FIFO_NAME_LEN];
	struct request req;
	struct response res;
	int seqNum = 0;		/* the service of this server, to provide this to client*/

	/* create a well-known FIFO and open it for reading */
	umask(0);
	if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWGRP | S_IWUSR) == -1 && errno != EEXIST) {
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}
	
	serverFd = open(SERVER_FIFO , O_RDONLY);
	if (serverFd == -1) {
		perror("server open");
		exit(1);
	}

	/* open extra write FD so that reader never sees EOF */
	dummyFd = open(SERVER_FIFO, O_WRONLY);
	if (dummyFd == -1) {
		perror("server open");
		exit(1);
	}

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		perror("signal");
		exit(1);
	}

	/* read requests and send responses */
	for(;;) {
		if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
			fprintf(stderr, "Error reading request: discarding\n");
			continue;
		}

		/* open client FIFO prev prepared by the client */
		snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)req.pid);
		clientFd = open(clientFifo, O_WRONLY);
		if (clientFd == -1) {
			fprintf(stderr, "open %s", clientFifo);
			continue;
		}

		/* sends response to FIFO and close the FIFO */
		res.seqNum = seqNum;
		if (write(clientFd, &res, sizeof(struct response)) != sizeof(struct response)) {
			fprintf(stderr, "Error writing to the FIFO %s\n", clientFifo);
		}
		if (close(clientFd) == -1) perror("close");

		seqNum += req.seqLen; 	// this is the service that is returned
	}

}
