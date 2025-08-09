#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s mq-name\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (mq_unlink(argv[1]) == -1) {
		perror("mq_unlink");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
