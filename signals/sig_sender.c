/* The purpose of this program is to send multiple signals to another process 
 * showcasing that signals are not queued.
 * Infact, the standard (non-RT) signals are not queued,but maintained as a
 * set, so if while being blocks N number of M signals were sent to process 
 * the process would only have M signals with 1 occurence each
 * It does not record how many times a specific signal was received
 */

#include "signal_functions.h"
#include <string.h>

int main(int argc, char *argv[])
{
  int numSigs, sig, j;
  pid_t pid;

  if (argc < 4 || strcmp(argv[1], "--help") == 0){
    fprintf(stderr, "%s pid num-sigs sig-num [sig-num-2]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  pid = atol(argv[1]);
  numSigs = atoi(argv[2]);
  sig = atoi(argv[3]);

  // send signals to the receiver
  printf("%s: sending signal %d to the receiver process %ld %d times\n",
      argv[0], sig, (long)pid, numSigs);

  for (j = 0; j < numSigs; j++){
    if (kill(pid, sig) == -1){
      perror("kill-signal-sending");
      exit(EXIT_FAILURE);
    }
  }

  if (argc > 4){
    if (kill(pid, atoi(argv[4])) == -1){
      perror("kill-signal-sending-2");
      exit(EXIT_FAILURE);
    }
  }

  printf("%s: exiting\n", argv[0]);
  exit(EXIT_SUCCESS);
}
