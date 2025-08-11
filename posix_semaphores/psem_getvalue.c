#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int value;
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

    if (sem_getvalue(sem, &value) == -1) {
        perror("sem_getvalue");
        exit(1);
    }

	printf("%d\n", value);
    exit(0);
}
