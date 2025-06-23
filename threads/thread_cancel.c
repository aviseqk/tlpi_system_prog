#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static void *threadFunc(void *arg)
{
	int j;
	printf("New thread started\n");
	for (j = 1; ; j++) {
		printf("Loop %d\n", j);
		sleep(1);
	}

	// NOT REACHED
	return NULL;
}

int main(int argc, char *argv[])
{
	pthread_t thr;
	int s;
	void *res;

	s = pthread_create(&thr, NULL, threadFunc, NULL);
	if (s != 0){
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	sleep(3);

	s = pthread_cancel(thr);
	if (s != 0) {
		perror("pthread_cancel");
		exit(EXIT_FAILURE);
	}

	s = pthread_join(thr, &res);
	if (s != 0) {
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}

	if (res == PTHREAD_CANCELED)
		printf("Thread was cancelled\n");
	else
		printf("Thread was not cancelled(shouldnot have happened)\n");

	exit(EXIT_SUCCESS);
}
