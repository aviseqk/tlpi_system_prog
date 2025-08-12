#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MEM_SIZE 10


int main(int argc, char *argv[])
{
	char *addr;
	int fd;
	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
	fprintf(stderr, "%s file [new-value]\n", argv[0]);
	}
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
		perror("open");

	addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED)
		perror("mmap");
	if (close(fd) == -1) /* No longer need 'fd' */
		perror("close");
	printf("Current string=%.*s\n", MEM_SIZE, addr);
	/* Secure practice: output at most MEM_SIZE bytes */
	if (argc > 2) { /* Update contents of region */
		if (strlen(argv[2]) >= MEM_SIZE)
			fprintf(stderr, "'new-value' too large\n");
		memset(addr, 0, MEM_SIZE); /* Zero out region */
		strncpy(addr, argv[2], MEM_SIZE - 1);
		if (msync(addr, MEM_SIZE, MS_SYNC) == -1)
			perror("msync");
	
		printf("Copied \"%s\" to shared memory\n", argv[2]);
	}
	exit(EXIT_SUCCESS);
}
