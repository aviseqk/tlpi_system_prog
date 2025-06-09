/* This program showcases the need and use of allocating a stack for when the signal handler is invoked
* Issue -> The kernel creates a frame for a signal handler whenever it is invoked, however this may
happen that the stack grows beyond its size and then kernel generates a SIGSEGV signal for that.
Now, since the kernel cannot create any stack for this SIGSEGV signal handler because of no space available,
it ends up terminating the process, as that is the default action of SIGSEGV handler.

* Solution -> We allocate a different area of memory, called alternate signal stack and let kernel know
of it. This way, frame for specified signal handler would be created on this altstack, not on process stack

* System Call -> sigaltstack() -> establishes an alternate signal stack and returns an existing one
*/

#define _GNU_SOURCE
#include "signal_functions.h"
#include <string.h>
#include <signal.h>

static void sigsegvHandler(int sig)
{
  int x;

  printf("Caught signal %d (%s)\n", sig, strsignal(sig)); //UNSAFE
  printf("Top of handler stack near    %10p\n", (void *)&x);
  fflush(NULL);

  _exit(EXIT_FAILURE);  
}

// A recursive function that overflows the stack
static void overflowStack(int callNum)
{
  char a[100000];

  printf("Call %4d - top of stack near %10p\n", callNum, &a[0]);
  overflowStack(callNum+1);
  
}

int main(int argc, char *argv[])
{
  stack_t sigstack;
  struct sigaction sa;
  int j;

  printf("Top of standard stack is near %10p\n", (void *)&j);

  // allocate alternate stack and inform kernel of its existence
  sigstack.ss_sp = malloc(SIGSTKSZ);
  if (sigstack.ss_sp == NULL){
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  sigstack.ss_size = SIGSTKSZ;
  sigstack.ss_flags = 0;

  if (sigaltstack(&sigstack, NULL) == -1){
    perror("sigaltstack");
    exit(EXIT_FAILURE);
  }

  printf("Alternate stack is at %10p-%p\n",sigstack.ss_sp, (char *)sbrk(0) - 1);

  // establish handler for SIGSEGV
  sa.sa_handler = sigsegvHandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_ONSTACK;

  if(sigaction(SIGSEGV, &sa, NULL) == -1){
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  overflowStack(1);
}
