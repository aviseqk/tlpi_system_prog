
/* The purpose of this program is to demonstrate the use of poll() system call, by creating a number of pipes, and randomly selected pair of file descriptors is used by poll()
 * to see if the file descriptor is ready for any I/O
 *
 * e.g. ./poll_pipes 10 3 -> ten pipes to be created and 3 random pipes to be tested for I/O readiness by writing to them
 * */


#include <time.h>
#include <sys/poll.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int numPipes, j, ready, randPipe, numWrites;
	int (*pfds)[2];		/* file descriptors for all the pipes */
	struct pollfd *pollFd;


	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s num-pipes [num-writes]\n",argv[0]);
		exit(EXIT_FAILURE);	
	}

	/* allocate the arrays that we use, as they are sized according to number of pipes given in command line arguments */
	numPipes = atoi(argv[1]);

	pfds = calloc(numPipes, sizeof(int [2]));
	if (pfds == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	pollFd = calloc(numPipes, sizeof(struct pollfd));
	if (pollFd == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}

	/* create the number of pipes specified */
	for (j = 0; j < numPipes; j++) {
		if (pipe(pfds[j]) == -1) {
			fprintf(stderr, "pipe %d", j);
			exit(EXIT_FAILURE);
		}
	}

	/* perform specified number of writes to random Pipes */
	numWrites = (argc > 2) ? atoi(argv[2]) : 1;
	srandom((int) time(NULL));
	if (pfds == NULL) {
		perror("calloc");
		exit(EXIT_FAILURE);
	}
	for (j= 0; j < numWrites; j++) {
		randPipe = random() % numPipes;
		printf("Writing to fd: %3d (read fd: %3d)\n", pfds[randPipe][1], pfds[randPipe][0]);
		if (write(pfds[randPipe][1], "a", 1) == -1) {
			fprintf(stderr, "write %d", pfds[randPipe][1]);
			exit(EXIT_FAILURE);
		}
	}

	/* build the file descriptor list to be passed to poll() call. This list to contain fd for read ends of all the pipes */
	for (j = 0; j < numPipes; j++) {
		pollFd[j].fd = pfds[j][0];
		pollFd[j].events = POLLIN;
	}

	ready = poll(pollFd, numPipes, 0);
	if (ready == -1) {
		perror("poll");
		exit(EXIT_FAILURE);
	}

	printf("poll() returned: %d\n", ready);

	/* check which pipes have data available for reading */
	for (j = 0; j < numPipes; j++) {
		if (pollFd[j].revents & POLLIN)
			printf("Readable: %3d\n", pollFd[j].fd);
	}

	exit(EXIT_SUCCESS);

}
