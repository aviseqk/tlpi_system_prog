/* The purpose of this program is to showcase how vfork works and its semantics */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  int istack = 222;    // allocated in stack segment
  pid_t childPid;

  switch(vfork()) {
    case -1:
      perror("vfork");
      exit(EXIT_FAILURE);

    case 0:    // child executes first, in parents memory space
      sleep(3);    // even if we sleep first, child executes first before parent is scheduled
      
      write(STDOUT_FILENO, "Child executing\n", 16);
      istack *= 3;
      _exit(EXIT_SUCCESS);

    default:
      write(STDOUT_FILENO, "Parent executing\n", 17);    // parent is blocked until child exits
      printf("istack=%d\n", istack);
      exit(EXIT_SUCCESS);
  }
}

