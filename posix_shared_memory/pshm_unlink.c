#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
fprintf(stderr, "%s shm-name\n", argv[0]);
	exit(EXIT_FAILURE);
}
if (shm_unlink(argv[1]) == -1) {
perror("shm_unlink");
exit(EXIT_FAILURE);
}
exit(EXIT_SUCCESS);
}
