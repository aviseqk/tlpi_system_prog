#include <mqueue.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void usageError(const char *programName)
{
    fprintf(stderr, "Usage: %s [-n] name msg [-prio]\n", programName);
    fprintf(stderr, "   -n      USE O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int flags, opt;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    ssize_t numRead;

    flags = O_RDONLY;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        switch(opt) {
            case 'n':   flags |= O_NONBLOCK;    break;
            default:    usageError(argv[0]);
        }
    }

    if (optind > argc)
        usageError(argv[0]);

    mqd = mq_open(argv[optind], flags);
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

    numRead = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
    if (numRead == -1) {
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }

    printf("Read %ld bytes; priority = %u\n", (long)numRead, prio);
    if (write(STDOUT_FILENO, buffer, numRead) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, "\n",1);

    exit(EXIT_SUCCESS);
}
