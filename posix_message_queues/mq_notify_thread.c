//* This is an example of message notification in POSIX Message Queues using Threads */
/* This program does the following things same as its signal counterpart except that message notifications are delivered by invocation of the thread functions in a separate thread and the main program remains blocked indefinitely
 * */

#include <signal.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

static void notifySetup(mqd_t *mqdp);

static void threadFunc(union sigval sv)
{
    ssize_t numRead;
    mqd_t *mqdp;
    void *buffer;
    struct mq_attr attr;

    mqdp = sv.sival_ptr;

    if (mq_getattr(*mqdp, &attr) == -1){
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    notifySetup(mqdp);

    while((numRead = mq_receive(*mqdp, buffer, attr.mq_msgsize, NULL)) >= 0)
        printf("Read %ld bytes\n", numRead);

    if (errno != EAGAIN) {
        perror("unexpected error mq_receive");
        exit(EXIT_FAILURE);
    }

    free(buffer);
}

static void notifySetup(mqd_t *mqdp)
{
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = threadFunc;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = mqdp;

    if (mq_notify(*mqdp, &sev) == -1) {
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    mqd_t mqd;
	
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s mq-name\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    
    notifySetup(&mqd);
    pause();
}

