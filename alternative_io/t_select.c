/* The purpose of this program is to demonstrate the use of select() system call, where via command line arguments we accpet file descriptors their kind of monitoring,
 * the timeout and then call select() to check readiness of the mentioned FDs
 *
 * e.g. ./t_select 10 0r -> monitor fd 0 for ready for reading with a 10-second timeout
 * 	./t_select 0 0r -> monitor fd 0 for ready for reading with a 0-second timeout
 * 	./t_select - 0r 1w -> monitor fd 0 for reading readiness and fd 1 for writing readiness with NULL timeout, so indefinite block until their readiness state changes
 *
 * */

#include <sys/time.h>
#include <sys/select.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static void usageError(const char *programName)
{
	fprintf(stderr, "Usage:	%s {timeout|-} fd-num[rw]...\n", programName);
	fprintf(stderr, "	- means infinite timeout; \n");
	fprintf(stderr, "	r = monitor for read\n");
	fprintf(stderr, "	w = monitor for write\n");
	fprintf(stderr, "	e.g.: %s - 0rw 1w\n", programName);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	fd_set readfds, writefds;
	int ready, nfds, fd, numRead, j;
	struct timeval timeout;
	struct timeval *pto;
	char buf[10];		/* large enough to hold "rw\0" */

	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageError(argv[0]);

	/* timeout for select() is in argv[1] */
	if (strcmp(argv[1], "-") == 0)
		pto = NULL;	// infinite timeout
	else {
		pto = &timeout;
		timeout.tv_sec = atol(argv[1]);
		timeout.tv_usec = 0;
	}

	/* process remaining arguments to build the file descriptor sets */
	nfds = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	for (j = 2; j < argc; j++) {
		numRead = sscanf(argv[j], "%d%2[rw]", &fd, buf);
		if (numRead != 2)
			usageError(argv[0]);
		if (fd >= FD_SETSIZE){
			fprintf(stderr, "file descriptor exceeds limit (%d)\n", FD_SETSIZE);
			exit(EXIT_FAILURE);
		}
		if (fd >= nfds)
			nfds = fd + 1;
		if (strchr(buf, 'r') != NULL)
			FD_SET(fd, &readfds);
		if (strchr(buf, 'w') != NULL)
			FD_SET(fd, &writefds);
	}

	/* now to call the select() system call */
	ready = select(nfds, &readfds, &writefds, NULL, pto);

	if (ready == -1){
		perror("select");
		exit(EXIT_FAILURE);
	}

	/* display results of select */
	printf("ready = %d\n", ready);

	for (fd = 0; fd < nfds; fd++) {
		printf("%d: %s%s\n", fd, FD_ISSET(fd, &readfds) ? "r" : "", FD_ISSET(fd, &writefds) ? "w" : "");
	}

	if (pto != NULL)
		printf("timeout after select() (as it returned before timeout): %ld.%03ld\n",
	 (long)timeout.tv_sec, (long) timeout.tv_usec / 1000);

	exit(EXIT_SUCCESS);
}
