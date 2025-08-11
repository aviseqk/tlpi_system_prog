/* This program is a command line program that lets us create a named semaphore, and then the examine the contents of Linux-specific virtual directory that contains named semaphores */

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void usageError(const char *programName)
{
    fprintf(stderr, "Usage: %s [-cx] name [octal-perms [value]]\n", programName);
    fprintf(stderr, "       -c  Create Semaphore (O_CREAT)\n");
    fprintf(stderr, "       -x  Create exclusively (O_EXCL)\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int flags, opt;
    mode_t perms;
    unsigned int value;
    sem_t *sem;

    flags = 0;
    while((opt=getopt(argc, argv, "cx")) != -1) {
        switch(opt) {
            case 'c':	flags |= O_CREAT;   break;
            case 'x':	flags |= O_EXCL;    break;
            default:	usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    /* default permissions are rw------- ; default semaphore initialization is to value of zero 0*/

    perms = (argc <= optind + 1) ? (S_IRUSR | S_IWUSR) : atoi(argv[optind + 1]);

    value = (argc <= optind + 2) ? 0 : atoi(argv[optind+2]);

    sem = sem_open(argv[optind], flags, perms, value);
    
    // if (sem == (sem_t *) 0) {
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
