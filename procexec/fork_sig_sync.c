#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include "../time/curr_time.h"

#define SYNC_SIG SIGUSR1 

static void handler(int sig)
{
  // Signal Handler - does nothing but return
}

int main(int argc, char *argv[])
{
  pid_t childPid;
  sigset_t blockMask, origMask, emptyMask;
  struct sigaction sa;

  setbuf(stdout, NULL);

  sigemptyset(&blockMask);
  sigaddset(&blockMask, SYNC_SIG);
  if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sa.sa_handler = handler;

  if(sigaction(SYNC_SIG, &sa, NULL) == -1){
    perror("sigaction");
    exit(EXIT_FAILURE);
  }

  childPid = fork();
  switch (childPid) {
    case -1:
      perror("fork");
      exit(EXIT_FAILURE);

    case 0:
      printf("[%s %ld] Child started - doing some work\n", currTime("%T"), (long) getpid());
      sleep(2);    // simulate time spending doing some work

      // signals parent that it is done
      printf("[%s %ld] Child about to signal parent\n", currTime("%T"), (long) getpid());

      if (kill(getppid(), SYNC_SIG) == -1){
        perror("kill");
        exit(EXIT_FAILURE);
      }

      _exit(EXIT_SUCCESS);

    default:
      // parent does some work here and waits for the child to complete
      printf("[%s %ld] Parent about to wait for signal\n", currTime("%T"), (long) getpid());

      sigemptyset(&emptyMask);
      if (sigsuspend(&emptyMask) == -1 && errno != EINTR){
        perror("sigsuspend");
        exit(EXIT_FAILURE);
      }

      printf("[%s %ld] Parent got signal\n", currTime("%T"), (long) getpid());

      if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1){
        perror("sigprocmask");
        exit(EXIT_FAILURE);
      }

      exit(EXIT_SUCCESS);
  }
}
