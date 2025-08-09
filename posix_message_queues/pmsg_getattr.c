#include <mqueue.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	mqd_t mqd;
	struct mq_attr attr;

	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s mq-name\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mqd = mq_open(argv[1], O_RDONLY);
	if (mqd == (mqd_t) -1) {
		perror("mq open");
		exit(EXIT_FAILURE);
	}

	if (mq_getattr(mqd, &attr) == -1) {
		perror("mq_getattr");
		exit(EXIT_FAILURE);
	}

	printf("Maximum # of messages on the queue:	%ld\n", attr.mq_maxmsg);
	printf("Maximum message size:			%ld\n", attr.mq_msgsize);
	printf("# of messages currently on the queue:	%ld\n", attr.mq_curmsgs);
	exit(EXIT_SUCCESS);
}
