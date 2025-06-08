/* The purpose of this program is to catch and count signals 
 * if and when it received the SIGINT it ends the process otherwise runs a main loop that uses CPU time */

#define _GNU_SOURCE
#include <signal.h>
#include "signal_functions.h"

static int sigCnt[NSIG];  // array to count deliveries of each signal
static volatile sig_atomic_t gotSigint = 0;   // set to nonZero if SIGINT is delivered

static void sigHandler(int sig)
{
  if (sig == SIGINT)
    gotSigint = 1;
  else
    sigCnt[sig]++;
}

int main(int argc, char *argv[])
{
  int n, numSecs;
  sigset_t pendingMask, blockingMask, emptyMask;

  printf("%s: PID is %ld\n", argv[0], (long)getpid());

  // setup the same handler for all the signals
  for (n=1;n<NSIG;n++)
    (void) signal(n, sigHandler); // void call as to ignore the errors received from signal() for SIGKILL & SIGSTOP
  
  // if a sleep time was specified in the command line argument, block all the signals, 
  // sleep for that time (while another process keeps sending signals), and then display the mask of pending
  // signals and then unblock all the signals
  
  if (argc > 1) {
    numSecs = atoi(argv[1]);

    sigfillset(&blockingMask);
    if (sigprocmask(SIG_BLOCK, &blockingMask, NULL) == -1){ // all signals are blocked now
      perror("sigprocmask");
      exit(EXIT_FAILURE);
    }

    printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
    sleep(numSecs); // sleeping for n seconds
    
    // once awake get the pending signals
    if (sigpending(&pendingMask) == -1) {
      perror("sigpending");
      exit(EXIT_FAILURE);
    }

    printf("%s: pending signals are: \n", argv[0]);
    printSigset(stdout, "\t\t", &pendingMask);

    // unblock all the signals, by assigning a empty signal mask to this process
    sigemptyset(&emptyMask);
    if (sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1) {
      perror("sigprocmask");
      exit(EXIT_FAILURE);
    }

  } 

  // loop until SIGINT is caught
  while(!gotSigint)
    continue;

  // display the number of signals received
  for (n=1; n < NSIG; n++){
    if (sigCnt[n] != 0)
      printf("%s signal %d caught %d time%s\n",
          argv[0], n, sigCnt[n], (sigCnt[n] == 1) ? "" : "s");
  }

  exit(EXIT_SUCCESS);
}
