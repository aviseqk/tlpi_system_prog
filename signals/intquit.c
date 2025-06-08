/* This program demonstrates signal handling of multiple signals in the same signal handler */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void signalHandler(int sig)
{
  static int count = 0;
  
  // if the signal is SIGINT
  if (sig == SIGINT) {
    count++;
    printf("Caught SIGINT (%d)\n", count);
    return;
  }

  // if the signal is SIGQUIT, something else - print a message and terminate the process
  printf("Caught SIGQUIT - that's all\n");
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  // establish same handler for both SIGINT and SIGQUIT signals

  if (signal(SIGINT, signalHandler) == SIG_ERR){
    perror("signal-SIGINT");
    exit(EXIT_FAILURE);
  }
  if (signal(SIGQUIT, signalHandler) == SIG_ERR){
    perror("signal-SIGQUIT");
    exit(EXIT_FAILURE);
  }

  // this loop basically loops forever waiting for signals and blocks until a signal is caught
  for (;;)
    pause();
}
