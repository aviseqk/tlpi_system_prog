#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "../time/curr_time.h"

int main(int argc, char *argv[])
{
	int numDead;
	pid_t childPid;
	int j;

	if (argc < 2 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr, "%s sleep-time...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	setbuf(stdout, NULL);

	for (j = 1; j < argc; j++){
		// create one child for each argument
		switch(fork()){
			case -1:
				perror("fork");
				exit(EXIT_FAILURE);

			case 0:
				printf("[%s] child %d started with PID %ld, sleeping for %s seconds\n",
					currTime("%T"), j, (long) getpid(), argv[j]);
				sleep(atoi(argv[j]));
				_exit(EXIT_SUCCESS);

			default:
				break;
		}
	}

	numDead = 0;
	for(;;){
		// parent waits for each child to exit
		childPid = wait(NULL);
		if (childPid == -1){
			if (errno == ECHILD){
				printf("No more children to kill - bye\n");
				exit(EXIT_SUCCESS);
			} else {
				perror("wait");
				exit(EXIT_FAILURE);
			}
		}

		numDead++;
		printf("[%s] wait() returned child PID %ld (numDead=%d)\n", currTime("%T"), (long) childPid, numDead);
	}
}
