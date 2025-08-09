/* This program provides a command line interface that lets us create a POSIX message queue. It takes two command line options, -m (for mq_maxmsg attr) and -s(for mq_msgsize attr) */
/* No argument means the implementation default will be passed on to the open() calls */


#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void usageError(const char *programName)
{
    fprintf(stderr, "Usage: %s [-cx] [-m maxmsg] [-s msgsize] mq-name " "[octal-perms]\n", programName);
    fprintf(stderr, "   -c      Create Queue (O_CREAT)\n");
    fprintf(stderr, "   -m  maxmsg  Set maximum # of messages\n");
    fprintf(stderr, "   -s msgsize Set maximum message size\n");
    fprintf(stderr, "   -x      Create exclusively (O_EXCL)\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int flags, opt;
    mode_t perms;
    mqd_t mqd;
    struct mq_attr attr, *attrp;

    /* setting the defaults */
    attrp = NULL;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 2048;
    flags = O_RDWR;

    /* parsing command-line arguments */

    while ((opt = getopt(argc, argv, "cm:s:x")) != -1)
    {
        switch(opt) {
            case 'c':
                flags |= O_CREAT;
                break;
            case 'm':
                attr.mq_maxmsg = atoi(optarg);
                attrp = &attr;
                break;
            case 's':
                attr.mq_msgsize = atoi(optarg);
                attrp = &attr;
                break;
            case 'x':
                flags |= O_EXCL;
                break;
            default:
                usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    perms = (argc <= optind + 1) ? (S_IRUSR | S_IWUSR) : atoi(argv[optind + 1]);

    mqd = mq_open(argv[optind], flags, perms, attrp);
    if (mqd == (mqd_t) -1){
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
