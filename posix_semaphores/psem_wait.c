#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	sem_t *sem;
    
	if (argc != 2 || strcmp(argv[1], "--help") == 0){
		perror("invalid command line arguments");
		exit(1);
	}

    sem = sem_open(argv[1], 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        exit(1);
    }

    printf("%ld sem_wait() succedded\n", (long) getpid());
    exit(0);
}
