/* This is an example program on how to use epoll API for monitoring multiple file descriptors for I/O */
/*
 * As command-line arguments this program excepts the pathnames of one or more terminals or FIFOs.
 * and then the program performs the following steps: 
 * 1. Create an epoll instance
 * 2. Open each of the files names on the cmd line for input and add the resulting fds to the interest list of the epoll instance specifying to be monitored as EPOLLIN
 * 3. Execute a loop tghat calls epoll+_wait() rto monitor the interest list of the epoll instance and handles the returned events from each call.
 * */


#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_BUF 1000
#define MAX_EVENTS 5

int main(int argc, char *argv[])
{
	int epfd, ready, fd, s, j, numOpenFds;
	struct epoll_event ev;
	struct epoll_event evlist[MAX_EVENTS];
	char buf[MAX_BUF];

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s file....\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	epfd = epoll_create(argc - 1);
	if (epfd == -1) {
		perror("epoll_create");
		exit(EXIT_FAILURE);
	}

	/* open each file on command line, and add it to the interest list for the epoll instance  */
	for (j=1 ; j < argc ; j++) {
		fd = open(argv[j], O_RDONLY);
		if (fd == -1) {
			perror("open");
			exit(EXIT_FAILURE);
		}
		printf("Opened \"%s\" on fd %d\n", argv[j], fd);

		ev.events = EPOLLIN;	/* only interested in INPUT events */
		ev.data.fd = fd;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
			perror("epoll_ctl");
			exit(EXIT_FAILURE);
		}
	}

	numOpenFds = argc - 1;

	while(numOpenFds > 0) {
		/* fetch upto MAX_EVENTS items from the ready list */
		printf("About to call epoll_wait()\n");
		ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
		if (ready == -1) {
			if (errno == EINTR)
				continue;	/* restart if interrupted by a signal */
			else {
				perror("epoll-wait");
				exit(EXIT_FAILURE);
			}
		}
		printf("Ready: %d\n", ready);

		/* deal with returned list of events */

		for (j =0; j < ready; j++) {
			printf("  fd=%d; events: %s%s%s\n", evlist[j].data.fd,
	  			(evlist[j].events & EPOLLIN) ? "EPOLLIN " : "",
	  			(evlist[j].events & EPOLLHUP) ? "EPOLLHUP " : "",
	  			(evlist[j].events & EPOLLERR) ? "EPOLLERR" : "");

			if (evlist[j].events & EPOLLIN) {
				s = read(evlist[j].data.fd, buf, MAX_BUF);
				if (s == -1) {
					perror("read");
					exit(EXIT_FAILURE);
				}
				printf("  read %d bytes: %.*s\n", s, s, buf);
			} else if (evlist[j].events & (EPOLLHUP | EPOLLERR)) {
				// case of receiving EPOLLHUP or EPOLLERR when not received any EPOLLIN, as this is an else if clause
				/* after the epoll_wait(). both EPOLLIN and EPOLLHUP may both have been set, but we will only get here, and close the fd if EPOLLIN was not set 
				 * This ensures that all the outstanding input (whatever is more than PIPE_BUF) is consumed by further loop iterations  before the file descriptor is closed */
				
				printf("  closing fd %d\n", evlist[j].data.fd);
				if (close(evlist[j].data.fd) == -1) {
					perror("close");
					exit(EXIT_FAILURE);
				}
				numOpenFds--;
			}
		}
	}
	printf("All file descriptors closed; bye\n");
	exit(EXIT_SUCCESS);
}
