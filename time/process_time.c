/*
 * This program displays the process time taken by a process using the clock() library function call 
 */

#include <sys/times.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

// Display 'msg' and process times
static void displayProcessTimes(const char *msg)
{
    struct tms t;
    clock_t clockTime;
    static long clockTicks = 0;

    if (msg!=NULL)
        printf("%s", msg);

    if (clockTicks == 0){
        clockTicks = sysconf(_SC_CLK_TCK);
        if (clockTicks == -1){
            perror("sysconf");
            exit(-1);
        }
    }

    clockTime = clock();
    if (clockTime == -1){
        perror("clock");
        exit(-1);
    }

    printf("clock() returns: %ld clocks-per-sec (%.2f secs)\n", 
    (long) clockTime, (double) clockTime / CLOCKS_PER_SEC);

    if (times(&t) == -1){
        perror("times");
        exit(-1);
    }
    printf("times() yields: user CPU=%.2f; system CPU=%.2f\n",
    (double) t.tms_utime / clockTicks,
    (double) t.tms_stime / clockTicks);
}

int main(int argc, char *argv[])
{
    int numCalls, j;

    printf("CLOCKS_PER_SEC=%ld sysconf(_SC_CLK_TCK)=%ld\n\n",
    (long)CLOCKS_PER_SEC, sysconf(_SC_CLK_TCK));

    displayProcessTimes("At program start: \n");

    numCalls = (argc > 1) ? atoi(argv[1]) : 10000000;
    for (j = 0; j < numCalls; j++)
        (void) getppid();

    displayProcessTimes("After the getppid() calls:\n");

    exit(EXIT_SUCCESS);
}
