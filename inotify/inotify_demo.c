/* This program explains the simple use of inotify API*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <limits.h>
#include <string.h>./d

// display information from inotify_event structure
static void displayInotifyEvent(struct inotify_event *i)
{
    printf("    wd=%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS )        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB )        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE ) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE )   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE )        printf("IN_CREATE ");
    if (i->mask & IN_DELETE )        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF )   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED )       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR )         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY )        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF )     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM )    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO )      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN )          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW )    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT )       printf("IN_UNMOUNT ");
    printf("\n");

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

#define BUF_LEN ( 10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int main(int argc, char *argv[])
{
    int inotifyFd, wd, j;
    char buf[BUF_LEN];
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    if (argc < 2 || strcmp(argv[1], "--help") == 0){
        printf("%s pathname.... \n", argv[0]);
        perror("arguments issue");
        exit(-1);
    }

    // create inotify instance
    inotifyFd = inotify_init();
    if (inotifyFd == -1){
        perror("inotify-init");
        exit(EXIT_FAILURE);
    }

    // add watch on the created inotify instance
    for (j = 1; j < argc; j++){
        wd = inotify_add_watch(inotifyFd, argv[j], IN_ALL_EVENTS);
        if (wd == -1){
            perror("inotify_add_watch");
            exit(EXIT_FAILURE);
        }

        printf("Watching %s using wd %d\n", argv[j], wd);
    }

    // read events forever till application closes
    for (;;){
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0){
            perror("read() from inotify fd returned 0!");
            exit(EXIT_FAILURE);
        }
        if (numRead == -1){
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("Read %ld bytes from inotify fd\n", (long) numRead);

        // process all events in buffer returned by read()

        for (p = buf; p < buf + numRead; ) {
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);

            p += sizeof(struct inotify_event) + event->len;
        }
    }

    exit(EXIT_SUCCESS);
}