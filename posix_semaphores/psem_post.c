#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	sem_t *sem;
    
	if (argc != 2 ){
		perror("invalid command line arguments");
		exit(1);
	}

    sem = sem_open(argv[1], 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    if (sem_post(sem) == -1) {
        perror("sem_post");
        exit(1);
    }

    exit(0);
}
