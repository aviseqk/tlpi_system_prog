#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int fd, flags;
	char template[] = "/tmp/testXXXXXX";

	setbuf(stdout, NULL);    // disable buffering of stdout

	fd = mkstemp(template);
	if (fd == -1) {
	  perror("mkstemp");
	  exit(EXIT_FAILURE);
	}

	printf("File offset before fork(): %lld\n", (long long) lseek(fd, 0, SEEK_CUR));

	flags = fcntl(fd, F_GETFL);
	if (flags == -1){
	  perror("fcntl - F_GETFL");
	  exit(EXIT_FAILURE);
	}
	printf("O_APPEND flag before fork() is: %s\n", ( flags & O_APPEND ) ? "on" : "off");

	switch(fork()) {
	  case -1:
	    perror("fork");
	    exit(EXIT_FAILURE);

	  case 0:
	    if (lseek(fd, 1000, SEEK_SET) == -1){
	      perror("lseek");
	      exit(EXIT_FAILURE);
	    }

	    flags = fcntl(fd, F_GETFL);
	    if (flags == -1){
	      perror("fcntl -F_GETFL");
	      exit(EXIT_FAILURE);
	    }
	    flags |= O_APPEND;
	    if (fcntl(fd, F_SETFL, flags) == -1){    // assign changed flag
	      perror("fcntl -F_SETFL");
	      exit(EXIT_FAILURE);
	    }
	    _exit(EXIT_SUCCESS);

	  default:
	    if (wait(NULL) == -1) {    // wait for child process to exit
	      perror("wait");
	      exit(EXIT_FAILURE);
	    }
	    printf("Child has exited\n");

	    printf("File offset in parent: %lld\n", (long long) lseek(fd, 0, SEEK_CUR));

	    flags = fcntl(fd, F_GETFL);
	    if (flags == -1){
	      perror("fcntl - F_GETFL");
	      exit(EXIT_FAILURE);
	    }

	    printf("O_APPEND FLAG in parent is %s\n", (flags & O_APPEND) ? "on" : "off");
	    exit(EXIT_SUCCESS);
	}
}
