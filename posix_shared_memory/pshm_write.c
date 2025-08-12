#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd;
	size_t len;	/* size of the shared memory object */
	char *addr;

	if (argc != 3 || strcmp(argv[1], "--help") == 0) {
		perror("invalid command line arguments");
		exit(EXIT_FAILURE);
	}

	/* Open existing object */
	fd = shm_open(argv[1], O_RDWR, 0);
	if (fd == -1) {
		perror("shm_+open");
		exit(EXIT_FAILURE);
	}

	len = strlen(argv[2]);
	/* Resize the object to hold the string */
	if (ftruncate(fd, len) == -1) {
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}
	printf("Resized to %ld bytes\n", (long) len);
	
	/* resized hence remapping */
	addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	if (close(fd) == -1) {
	perror("close");
	exit(EXIT_FAILURE);
	}

	printf("copying %ld bytes \n", (long) len);
	

	/* copy string to shared memory */
	memcpy(addr, argv[2], len);
	exit(EXIT_SUCCESS);
}
