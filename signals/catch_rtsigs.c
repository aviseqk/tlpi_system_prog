#define _GNU_SOURCE
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include "signal_functions.h"

static volatile int handlerSleepTime;
static volatile int sigCnt = 0;
static volatile sig_atomic_t allDone = 0;

static void siginfoHandler(int sig, siginfo_t *si, void *ucontext)
{
  
  if (sig == SIGINT || sig == SIGTERM) {
    allDone = 1;
    return;
  }

  sigCnt++;
  printf("caught a signal %d\n", sig);    // UNSAFE -> uses non-(async-signal-safe) fx like printf

  printf("    si_signo=%d, si_code=%d (%s), ", si->si_signo, si->si_code,
         (si->si_code == SI_USER) ? "SI_USER" :
       (si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
  printf("si_value=%d\n", si->si_value.sival_int);
  printf("    si_pid=%ld, si_uid=%ld\n", (long)si->si_pid, (long)si->si_uid);

  sleep(handlerSleepTime);
}

int main(int argc, char *argv[])
{
  struct sigaction sa;
  int sig;
  sigset_t prevMask, blockMask;

  if (argc > 1 || strcmp(argv[1], "--help") == 0){
    fprintf(stderr, "%s [block-time [handler-sleep-time]]\n", argv[0]);

  printf("%s: PID is %ld\n", argv[0], (long) getpid());

  handlerSleepTime = (argc > 2) ? atoi(argv[2]) : 1;

  // establish handler for most signals
  // during execution, mask all other signals to prevent handlers recursively interrupting each other

  sa.sa_sigaction = siginfoHandler;
  sa.sa_flags = SA_SIGINFO;
  sigfillset(&sa.sa_mask);

  for (sig = 1; sig < NSIG; sig++){
    if (sig != SIGSTOP && sig != SIGQUIT)
      sigaction(sig, &sa, NULL);
  }

  // optionally block signals and sleep allowing signals to be sent before they are unblocked and handled

  if (argc > 1){
    sigfillset(&blockMask);
    sigdelset(&blockMask, SIGINT);
    sigdelset(&blockMask, SIGTERM);

    if (sigprocmask(SIG_SETMASK, &blockMask, &prevMask) == -1){
      perror("sigprocmask");
      exit(EXIT_FAILURE);
    }

    printf("%s: signals blocked - sleeping %s seconds\n", argv[0], argv[1]);
    sleep(atoi(argv[1]));
    printf("%s: sleep complete\n", argv[0]);

    if (sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1){
      perror("sigprocmask");
      exit(EXIT_FAILURE);
    }
  }

  while(!allDone)
    pause();

  printf("Caught %d signals\n", sigCnt);
  exit(EXIT_SUCCESS);
}
}
