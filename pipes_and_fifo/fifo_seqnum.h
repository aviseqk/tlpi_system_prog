#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_FIFO "/tmp/seqnum_server"	/* well-known name for server's FIFO */

#define CLIENT_FIFO_TEMPLATE "/tmp/seqnum_cl.%ld"	/* template for building client FIFO's name */

#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)	/* space required for the client's FIFO pathname + 20 allowance for PID */

struct request {	/* Request (client --> server) */
	pid_t pid;	/* PID of client */
	int seqLen;	/* Length of desired sequence */
};

struct response {	/* Response (server --> client) */
	int seqNum;	/* Start of sequence */
};
