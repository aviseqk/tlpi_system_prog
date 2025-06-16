#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void atexitFunc1(void)
{
  printf("atexit function 1 called\n");
}

static void atexitFunc2(void)
{
  printf("atexit function 2 called\n");
}

static void onexitFunc(int exitStatus, void *arg)
{
  printf("onexit function called: status = %d, arg = %ld\n", exitStatus, (long) arg);
}

int main(int argc, char *argv[])
{
  if (on_exit(onexitFunc, (void *) 10) != 0){
    perror("on_exit 1");
    exit(EXIT_FAILURE);
  }
  if (atexit(atexitFunc1) != 0){
    perror("atexit 1");
    exit(EXIT_FAILURE);
  }
  if (atexit(atexitFunc2) != 0){
    perror("atexit 2");
    exit(EXIT_FAILURE);
  }

  if (on_exit(onexitFunc, (void *) 20) != 0){
    perror("on_exit 2");
    exit(EXIT_FAILURE);
  }

  exit(2);
}
