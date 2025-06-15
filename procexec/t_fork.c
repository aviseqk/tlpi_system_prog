/* The purpose of this program is to showcase how fork gives the child processes its own copy
of stack and heap variables from the parent and it is able to modify those variables without
affecting the same for the parent */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static int idata = 111;    // allocated in data segment

int main(int argc, char *argv[])
{
  int istack = 222;    // allocated in stack segment
  pid_t childPid;

  switch(childPid = fork()) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);

    case 0:
      idata *= 3;
      istack *= 3;
      break;

    default:
      sleep(3);    // pause/sleep in parent process to let child be scheduled by CPU before parent
      break;
  }

  /* both parent and child processes will arrive here */
  printf("PID=%ld %s idata=%d istack=%d\n", (long)getpid(),
    (childPid == 0) ? "child" : "parent", idata, istack);
  exit(EXIT_SUCCESS);
}
