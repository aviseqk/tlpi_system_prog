#define _GNU_SOURCE
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include "print_wait_status.h"

/* examine a wait() status using W* macros */
void printWaitStatus(const char *msg, int status)
{
    if (msg != NULL)
        printf("%s", msg);

    if (WIFEXITED(status)){
        printf("child exited, status=%d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("child killed by signal %d(%s)", WTERMSIG(status), strsignal(WTERMSIG(status)));

    #ifdef WCOREDUMP
        if (WCOREDUMP(status))
            printf("(core dumped)");
    #endif
        printf("\n");
    
    } else if(WIFSTOPPED(status)) {
        printf("child stopped by signal %d (%s)\n", WSTOPSIG(status), strsignal(WSTOPSIG(status)));

    #ifdef WIFCONTINUED
        if (WIFCONTINUED(status))
            printf("child continued");
    #endif
        printf("\n");
    }
    else {
        printf("What happened to this child?, (status=%x)\n", (unsigned int) status);
    }
}