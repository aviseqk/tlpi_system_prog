// REFER TO TLPI LISTING 52-6 's end for the explanation of every step and why it was decided to take these steps

/* This is an example of message notification in POSIX Message Queues using Signals  */
/* This program does the following things
 * 1. Open a message queue from the command line argument in a non blocking mode, determine the mq_msgsize attribute for the message queue and allocate a buffer for receving messages
 * 2. Block the notification signal(SIGUSR1) and establish a handler for it
 * 3. Make a call to mq_notify to register itself to receive a notification
 * 4. Execute an infinite loop that does the following:
 * 	a.) Call sigsuspend which unblocks the signal and waits until the signal is caught. when this completes, it indicates the signal has received and this process is now deregistered for the notification now
 * 	b.) Call mq_notify() to reregister the process for notifications
 * 	c.) Execute a while loop that drains the message queue by reading as many messages as possible
 * */

#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define NOTIFY_SIG SIGUSR1

static void handler(int sig)
{
	/* just to interrupt sigsuspend() */
}

int main(int argc, char *argv[])
{
	struct sigevent sev;
	mqd_t mqd;
	struct mq_attr attr;
	void *buffer;
	ssize_t numRead;
	sigset_t blockMask, emptyMask;
	struct sigaction sa;

	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s mq-name\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
	if (mqd == (mqd_t) -1) {
		perror("mq_open");
		exit(EXIT_FAILURE);
	}
	
	if (mq_getattr(mqd, &attr) == -1) {
		perror("mq_getattr");
		exit(EXIT_FAILURE);
	}

	buffer = malloc(attr.mq_msgsize);
	if (buffer == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&blockMask);
	sigaddset(&blockMask, NOTIFY_SIG);
	if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1) {
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction(NOTIFY_SIG, &sa, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = NOTIFY_SIG;
	if (mq_notify(mqd, &sev) == -1) {
		perror("mq_notify");
		exit(EXIT_FAILURE);
	}

	sigemptyset(&emptyMask);

	for (;;) {
		sigsuspend(&emptyMask);

		if (mq_notify(mqd, &sev) == -1) {
			perror("mq_notify");
			exit(EXIT_FAILURE);
		}

		while((numRead = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0)
			printf("Read %ld bytes\n", (long) numRead);

		if (errno != EAGAIN) {
			perror("mq_receive - unaccounted error");
			exit(EXIT_FAILURE);
		}
	}

}
