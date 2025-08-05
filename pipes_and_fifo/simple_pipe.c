/*
 * The purpose of this program is to demonstrate a simple pipe. 
 * One process creates a pipe, then calls fork() to create a Child Process, then the Parent Process closes its read descriptor, and writes string
 * given in the command line to the pipe,. The parent then closes the write end of the pipe and calls wait() to wait for the child to terminate
 * The child process after creation closes the write end of the pipe, and then enters a loop to read from the pipe, in blocks dfined in BUF_SIZE, and writes
 * them to the standard output, once the child process encounters EOF, it closes its descriptor for the read end of the file, and terminates
 *
 * example Command :
 * ./simple_pipe 'It was a cold bright day in April, \
 * and the clocks were striking thirteen.'
 * */



#include <sys/wait.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 10

int main(int argc, char *argv[])
{
	int pfd[2];	// file descriptors opened by the pipe
	char buf[BUF_SIZE];
	ssize_t numRead;

	if (argc != 2 || strcmp(argv[1], "--help") == 0) {
		printf("%s string\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// create the pipe
	if (pipe(pfd) == -1)
		perror("pipe creation");

	// fork a process
	switch(fork()) {
	case -1:
		perror("fork");

	// child - reads from the pipe
	case 0:
		if (close(pfd[1]) == -1)	/* write end is unused, so close it*/
			perror("close - child");
		for (;;) {	/* read data from pipe, echo to stdout */
			numRead = read(pfd[0], buf, BUF_SIZE);
			if (numRead == -1)
				perror("read");
			if (numRead == 0)
				break;		/* reached the EOF */
			if (write(STDOUT_FILENO, buf, numRead) != numRead)
				perror("child - partial/failed write");
			}
		write(STDOUT_FILENO, "\n", 1);		/* add a newline at the end of loop */
	
		// close the read end of the pipe
		if (close(pfd[0]) == -1) {
			perror("close");
		}
		exit(EXIT_SUCCESS);

	// parent - writes to the pipe
	default: 
		if (close(pfd[0]) == -1)
			perror("close - parent");

		if (write(pfd[1], argv[1], strlen(argv[1])) != strlen(argv[1]))
			perror("parent - partial/failed write");

		if (close(pfd[1]) == -1)
			perror("close");	/* makes the child see EOF */

		wait(NULL);	/* wait for the child to finish */
		exit(EXIT_SUCCESS);
	}
}


