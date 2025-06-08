#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

static void signalHandler(int sig)
{
  printf("Ouch\n");
}

int main(int argc, char *argv[])
{
  int j;

  if (signal(SIGINT, signalHandler) == SIG_ERR){
    perror("signal");
    exit(-1);
  }

  for (j=0; ; j++) {
    printf("%d\n",j);
    sleep(3);
  }
}
