/* child_status.c

   Demonstrate the use of wait() and the W* macros for analyzing the child
   status returned by wait()

   Usage: child_status [exit-status]

   If "exit-status" is supplied, then the child immediately exits with this
   status. If no command-line argument is supplied then the child loops waiting
   for signals that either cause it to stop or to terminate - both conditions
   can be detected and differentiated by the parent. The parent process
   repeatedly waits on the child until it detects that the child either exited
   normally or was killed by a signal.
*/
#include <sys/wait.h>
#include "print_wait_status.h"          /* Declares printWaitStatus() */
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int
main(int argc, char *argv[])
{
    int status;
    pid_t childPid;

    if (argc > 1 && strcmp(argv[1], "--help") == 0){
        printf("%s [exit-status]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    switch (fork()) {
    case -1: 
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:             /* Child: either exits immediately with given status or loops waiting for signals */
        printf("Child started with PID = %ld\n", (long) getpid());
        if (argc > 1)                   /* Status supplied on command line? */
            exit(atoi(argv[1]));
        else                            /* Otherwise, wait for signals */
            for (;;)
                pause();
        exit(EXIT_FAILURE);             /* Not reached, but good practice */

    default:            /* Parent: repeatedly wait on child until it
                           either exits or is terminated by a signal */
        for (;;) {
            childPid = waitpid(-1, &status, WUNTRACED
#ifdef WCONTINUED       /* Not present on older versions of Linux */
                                                | WCONTINUED
#endif
                    );
            if (childPid == -1)
                perror("waitpid");
                exit(EXIT_FAILURE);

            /* Print status in hex, and as separate decimal bytes */

            printf("waitpid() returned: PID=%ld; status=0x%04x (%d,%d)\n",(long) childPid,(unsigned int) status, status >> 8, status & 0xff);
            printWaitStatus(NULL, status);

            if (WIFEXITED(status) || WIFSIGNALED(status))
                exit(EXIT_SUCCESS);
        }
    }
}
