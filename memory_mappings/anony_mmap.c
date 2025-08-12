/* The program below demonstrates the use of either MAP_ANONYMOUS or /dev/zero
to share a mapped region between parent and child processes. The choice of tech-
nique is determined by whether USE_MAP_ANON is defined when compiling the pro-
gram. The parent initializes an integer in the shared region to 1 prior to calling
fork(). The child then increments the shared integer and exits, while the parent
waits for the child to exit and then prints the value of the integer. */

#ifdef USE_MAP_ANON
#define _BSD_SOURCE /* Get MAP_ANONYMOUS definition */
#endif
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>



int main(int argc, char *argv[])
{
	int *addr; /* Pointer to shared memory region */
	#ifdef USE_MAP_ANON /* Use MAP_ANONYMOUS */
	addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (addr == MAP_FAILED)
	perror("mmap");
	#else /* Map /dev/zero */
		int fd;
		fd = open("/dev/zero", O_RDWR);
		if (fd == -1)
			perror("open");
		addr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (addr == MAP_FAILED)
			perror("mmap");
		if (close(fd) == -1) /* No longer needed */
			perror("close");
	#endif
		*addr = 1; /* Initialize integer in mapped region */
		switch (fork()) { /* Parent and child share mapping */
		case -1:
			perror("fork");
		case 0: /* Child: increment shared integer and exit */
			printf("Child started, value = %d\n", *addr);
			(*addr)++;
			if (munmap(addr, sizeof(int)) == -1)
				perror("munmap");
			exit(EXIT_SUCCESS);
		default: /* Parent: wait for child to terminate */
			if (wait(NULL) == -1)
				perror("wait");
		printf("In parent, value = %d\n", *addr);

		if (munmap(addr, sizeof(int)) == -1)
		perror("munmap");
	
	exit(EXIT_SUCCESS);
	}
}
