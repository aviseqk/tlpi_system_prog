/* This program demonstrates how we read a file from the /proc filesystem and if an argument is provided, 
 * how to update that file using C functions */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define MAX_LINE 100

int main(int argc, char *argv[])
{
  int fd;
  ssize_t n;
  char line[MAX_LINE];

  fd = open("/proc/sys/kernel/pid_max", (argc > 1) ? O_RDWR : O_RDONLY);
  if (fd == -1){
    perror("file-open");
    exit(-1);
  }

  n = read(fd, line, MAX_LINE);
  if (n == -1){
    perror("file-read");
    exit(-1);
  }

  if (argc > 1)
    printf("Old Value: ");
  printf("%.*s\n", (int) n, line);

  if (argc > 1)
  {
   if (write(fd, argv[1], strlen(argv[1])) != strlen(argv[1])){
    perror("write-failed");
    exit(-1);
   }

   // system lib fn is used to run a shell command, it creates a new process that runs a command in shell for caller
   system("echo /proc/sys/kernel/pid_max now contains "
          "`cat /proc/sys/kernel/pid_max`");
  }
  exit(EXIT_SUCCESS);
}

