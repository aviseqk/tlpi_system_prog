/*
 * The purpose of this program is to achieve synchronization between parent and multiple child processes by using pipes
 * The way that is done is that parent opens a pipe before creating multiple child processes, and then the children receives the 
 * file descriptor to the writing end of the pipe.
 * The children can then close the FD to the write end once they are done with the work, and if all the child processes have closed their
 * file descriptors and then parent does a read() call, it sees the EOF, thus stating synchronization is complete
 * */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	int filedes[2];
	int j, dummy_buf;
	struct timeval tv;
	if (argc < 2 || strcmp(argv[1], "--help") == 0){
		perror("wrong arguments");
		exit(EXIT_FAILURE);
	}
	
	setbuf(stdout, NULL);	// making the stdout unbuffered
	
	gettimeofday(&tv, NULL);
	printf("%d:%d Parent started\n", (int) tv.tv_sec, (int) tv.tv_usec);

	if (pipe(filedes) == -1) {
		perror("pipe error");
		exit(EXIT_FAILURE);
	}
	for (j = 1; j < argc; j++){
		switch(fork()) {
			case -1:
				perror("fork error");
			
			// This is the child
			case 0:
				if (close(filedes[0]) == -1) {	/* vchild closes the read end of the pipe */
					perror("child read close");
					exit(EXIT_FAILURE);
				}

				/* child simulates doing some work, and lets parent know its done */
				sleep((int)atoi(argv[j]));
				gettimeofday(&tv, NULL);
				printf("%d:%d child %d pid (%ld) closing\n", tv.tv_sec, tv.tv_usec, j, (long)getpid());
				
				if (close(filedes[1]) == -1) {
					perror("child - pipe close for write");
					exit(EXIT_FAILURE);
				}
				// child does other things and then exits gracefully
				_exit(EXIT_SUCCESS);

			default:
				break;
		}
	}

		/* parent comes here, now it should close the write end of the file, to read EOF from children */
		if (close(filedes[1]) == -1) {
			perror("parent - pipe close for write");
			exit(EXIT_FAILURE);
		}

		/* parent reads from the pipe, and expects an EOF */
		if (read(filedes[0], &dummy_buf, 1) != 0) {
			perror("parent - not see EOF");
			exit(EXIT_FAILURE);
		}

		gettimeofday(&tv, NULL);
		printf("%d:%d Parent (pid %ld) ready to go \n", (int)tv.tv_sec, (int)tv.tv_usec, (long)getpid());

		exit(EXIT_SUCCESS);

	}

