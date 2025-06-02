/* This program showcases the fpathconf() library function to retrieve and display the system set limits constants 
 * defined in the limits.h header file
 * This program takes input of the file descriptor via the command line */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

// Prints msg alongwith the fpathconf value for 'fd' and 'name' limit
static void fpathconfPrint(const char *msg, int fd, int name)
{
	long lim;
	errno = 0;
	lim = fpathconf(fd, name);
 	// call succeeded, limit is determinate
 	if (lim != -1) {
 		printf("%s %ld\n", msg, lim);
	} else {
 		if (errno == 0) // Call succeeded, limit indeterminate
 			printf("%s (indeterminate)\n", msg);
 		else {
		// Call failed
 			perror("fpathconf\n");
			exit(-1);
 		}
	}
}

int main(int argc, char *argv[])
{
	fpathconfPrint("_PC_NAME_MAX:         ", STDIN_FILENO, _PC_NAME_MAX);
	fpathconfPrint("_PC_PATH_MAX:         ", STDIN_FILENO, _PC_PATH_MAX);
	fpathconfPrint("_PC_PIPE_BUF:         ", STDIN_FILENO, _PC_PIPE_BUF);
	exit(EXIT_SUCCESS);
}
