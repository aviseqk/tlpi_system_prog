/* This program showcases how to send a realtime signal to another process.
   by using sigqueue() system call, which also has a sigval argument that lets this process
   send some data alongwith the signal to the receiving process
*/

#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
  int sig, numSigs, j, sigData;
  union sigval sv;

  if (argc < 4 || strcmp(argv[1], "--help") == 0){
    fprintf(stderr, "%s pid sig-num data [num-sigs]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // display our PID and UID, for the receiver handler to be configured a/c to this
  printf("%s: PID is %ld, UID is %ld\n", argv[0], (long)getpid(), (long)getuid());

  sig = atoi(argv[2]);
  sigData = atoi(argv[3]);
  numSigs = (argc > 4) ? atoi(argv[4]) : 1;

  for (j = 0; j < numSigs; j++){
    sv.sival_int = sigData + j;
    if (sigqueue(atoi(argv[1]), sig, sv) == -1){
      perror("sigqueue");
      exit(EXIT_FAILURE);
    }
  }

  exit(EXIT_SUCCESS);
  
}
