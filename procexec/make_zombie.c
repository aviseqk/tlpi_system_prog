#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <libgen.h>
#include <sys/types.h>

#define CMD_SIZE 200

int main(int argc, char *argv[])
{
    char cmd[CMD_SIZE];
    pid_t childPid;

    setbuf(stdout, NULL);

    printf("Parent PID=%ld\n", (long) getpid());

    childPid = fork();
    switch (childPid)
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);

    case 0:
        // child exiting immediately to become a zombie for the parent
        printf("Child (PID=%ld) exiting\n", (long) getpid());
        _exit(EXIT_SUCCESS);
    
    default:
        // parent process
        sleep(3);    // giving the child time to die
    
        snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
        printf("terminal command - %s", cmd);
        system(cmd);

        // send the sure KILL signal to the zombie
        if (kill(childPid, SIGKILL) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
        sleep(3);
        printf("After sending SIGKILL to zombie (PID=%ld):\n", (long)childPid);
        system(cmd);


        exit(EXIT_SUCCESS);
    }
}