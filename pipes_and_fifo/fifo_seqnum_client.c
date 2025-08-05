/*
 * The function of this client program is to do the following:
 * 1. create a FIFO used to receive a response from the server. This is done before sending the request to make sure FIFO exists the time  server attempts to open it and send a responbse message
 * 2. construct a message for the server containing the clients process ID and a number taken from optional cmdline arhument, specifying the length of the sequence that the cxlient wishes
 * the server to assign to it
 * 3. open the server FIFO and send the message to the server
 * open the client FIFO and read and print the servers response
 * */

#include "fifo_seqnum.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

static char clientFifo[CLIENT_FIFO_NAME_LEN];

static void removeFifo(void)
{
	unlink(clientFifo);
}

int main(int argc, char *argv[])
{
	int serverFd, clientFd;
	struct request req;
	struct response res;

	if (argc > 1 && strcmp(argv[1], "-help") == 0) {
		fprintf(stderr, "%s [seq-len]\n", argv[0]);
     		exit(EXIT_FAILURE);
     	}
	
     /* create our FIFO befoire sending a request to avoid a race */
     umask(0);
     snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)getpid());
     if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST) {
		fprintf(stderr, "mkfifo %s", clientFifo);
     		exit(1);
     }

     if (atexit(removeFifo) != 0) {
		perror("atexit");
		exit(1);
	}

	/* construct the request message , open serverFIFO and send the request */
	req.pid = getpid();
	req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;

	serverFd = open(SERVER_FIFO, O_WRONLY);
	if (serverFd == -1) {
		fprintf(stderr, "open %s", SERVER_FIFO);
		exit(1);
	}

	if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
		perror("cant write to the server");
		exit(1);
	}

	/* open our FIFO, read the message and display the response */
	clientFd = open(clientFifo, O_RDONLY);
	if (clientFd == -1) {
		fprintf(stderr, "open %s", clientFifo);
		exit(1);
	}

	if (read(clientFd, &res, sizeof(struct response)) != sizeof(struct response)) {
		fprintf(stderr, "cant read response from server");
		exit(1);
	}

	printf("%d\n", res.seqNum);
	exit(0);
}
