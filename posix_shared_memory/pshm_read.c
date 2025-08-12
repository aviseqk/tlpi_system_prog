#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd;
	char *addr;
	struct stat sb;

	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr,"%s shm-name\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	fd = shm_open(argv[1], O_RDONLY, 0); /* Open existing object */
	if (fd == -1){
	perror("shm_open");
	exit(EXIT_FAILURE);
	}
	/* Use shared memory object size as length argument for mmap() and as number of bytes to write() */
	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		exit(EXIT_FAILURE);
	}
	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	if (close(fd) == -1) { /* 'fd' is no longer needed */
	perror("close");
	exit(EXIT_FAILURE);
}
	write(STDOUT_FILENO, addr, sb.st_size);
	printf("\n");
	exit(EXIT_SUCCESS);
}
