/*This program craetes a shared memory object whose size is specified by a command line argument and maps the object into the process' virtual address space
 * uses shm_open, ftruncate, and mmap() system calls
 * */

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void usageError(const char *programName)
{
	fprintf(stderr, "Usage:	%s  [-cx]  name  size  [octal-perms]\n", programName);
	fprintf(stderr, "	-c  Create shared memory (O_CREAT)\n");
	fprintf(stderr, "	-x  Create exclusively (O_EXCL)\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int flags, opt, fd;
	mode_t perms;
	size_t size;
	void *addr;

	flags = O_RDWR;
	while ((opt = getopt(argc, argv, "cx")) != -1) {
		switch (opt) {
			case 'c': flags |= O_CREAT; break;
			case 'x': flags |= O_EXCL; break;
			default: usageError(argv[0]);
		}
	}

	if (optind + 1 >= argc) {
		usageError(argv[0]);
	}
	size = atol(argv[optind+1]);
	perms = (argc <= optind + 2) ? (S_IRUSR | S_IWUSR) : atol(argv[optind+2]);

	/* Create a shared memory object and set its size */

	fd = shm_open(argv[optind], flags, perms);
	if (fd == -1) {
		perror("shm_open");
		exit(EXIT_FAILURE);
	}

	if (ftruncate(fd, size) == -1) {
		perror("ftruncate");
		exit(EXIT_FAILURE);
	}

	/* Map the shared memory object */
	printf("%ld size ", (long)size);
	addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
