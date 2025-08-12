/* This program demonstrates the use of mmap() to create a private file mapping.  It maps the entire file named in its command line argument and 
 * then writes the contents of the mapping to standard output */

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	char *addr;
	int fd;
	struct stat sb;

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s file\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	/* Obtain the size of the file and specify the size of the mapping, and the size of buffer to be written */
	if (fstat(fd, &sb) == -1) {
		perror("fstat");
		exit(EXIT_FAILURE);
	}

	addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (addr == MAP_FAILED) {
		perror("mmap");
		exit(EXIT_FAILURE);
	}

	if (write(STDOUT_FILENO, addr, sb.st_size) != sb.st_size) {
		perror("partial/failed write");
		exit(EXIT_FAILURE);
	}
       exit(EXIT_SUCCESS);
}
