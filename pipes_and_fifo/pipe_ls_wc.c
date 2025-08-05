/* 
 * CONNECT `wc` and `ls` using a Pipe 
 * The purpose of this program is to demonstrate the techniques used to use pipes to connect filters
 * Filters are programs that use and can read from stdin and write to stdout - basically their stdin and stdout file descriptors 
 * are nbound to respective read and write ends of the pipe
 * The issue is the way file descriptors are assigned by a pipe is by using lowest free file descriptors, and if stdin and stdout are open (generally 0, 1)
 * the pipe gives the FDs like 3 or 4.
 * -- Hence this is accomplished by first closing the open stdout file descriptors, thus making 1 the lowest free file descriptors and then duplicating the file descriptor
 *  making the now lowest free FD for the write end
 *  Refer TLPI 44.4 for more details and edge scenarios
 *
 *  -- Program Use Case -> After building a pipe, this program creates two child processes, first child binds its standard output to the write end of the pipe, 
 *  and execs ls, and second child binds itself to the read end of pipe, and execs wc
 * */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int filedes[2];

	if (pipe(filedes) == -1) {
		perror("pipe open");
		exit(EXIT_FAILURE);
	}

	switch(fork()) {
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);

		case 0:		// this is the first child, execs `ls` to write to the pipe
			if (close(filedes[0]) == -1) {
			perror("close 1");
			exit(EXIT_FAILURE);
			}

			// duplicate stdout on the write end of the pipe, and closes duplicated file descriptor
			if (filedes[1] != STDOUT_FILENO) {
				if (dup2(filedes[1], STDOUT_FILENO) == -1) {
				perror("dup2 1");
				exit(EXIT_FAILURE);
				}
				if (close(filedes[1]) == -1){
				perror("close 2");
				exit(EXIT_FAILURE);
				}
			}

			execlp("ls", "ls", (char *) NULL);	// writes to the pipe
			perror("execlp ls");
			exit(EXIT_FAILURE);
			
		default:
			break;
	}

	switch(fork()) {
			case -1:
			perror("fork");
			exit(EXIT_FAILURE);

		case 0:		// this is the second child, execs `wc` to read from the pipe
			if (close(filedes[1]) == -1) {
			perror("close 3");
			exit(EXIT_FAILURE);
			}

			// duplicate stdin on the read end of the pipe, and closes duplicated file descriptor
			if (filedes[0] != STDIN_FILENO) {
				if (dup2(filedes[0], STDIN_FILENO) == -1) {
				perror("dup2 2");
				exit(EXIT_FAILURE);
				}
				if (close(filedes[0]) == -1){
				perror("close 4");
				exit(EXIT_FAILURE);
				}
			}

			execlp("wc", "wc", "-l", (char *) NULL);	// writes to the pipe
			perror("execlp wc");
			exit(EXIT_FAILURE);
			
		default:
			break;
	}

	/* Now parent closes unused file descriptors and waits for the children to finish */
	
	if (close(filedes[0]) == -1) {
		perror("close 5");
		exit(EXIT_FAILURE);
	}
	if (close(filedes[1]) == -1) {
		perror("close 5");
		exit(EXIT_FAILURE);
	}
	/* wait for both children individually as blocking calls */
	if (wait(NULL) == -1) {
		perror("wait 1");
		exit(EXIT_FAILURE);
	}
	if (wait(NULL) == -1) {
		perror("wait 2");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

