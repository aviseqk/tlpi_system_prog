#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc != 2 || strcmp(argv[1], "--help") == 0){
		perror("invalid command line arguments");
		exit(1);
	}

	if (sem_unlink(argv[1]) == -1) {
		perror("sem_unlink");
		exit(1);
	}
	exit(0);
}
