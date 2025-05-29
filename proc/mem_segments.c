/*
* The purpose of this program is to highlight via comments
* where each program variable lies in the memory layout
* or segments of a process.
*/

#include <stdio.h>
#include <stdlib.h>

char globBuf[65536];   /* Uninitialized Data Segment */
int primes[] = {2, 3, 5, 7};   /* Initialized Data Segment */

static int square(int x)    /* Allocated in stack frame of square() */
{
  int result;    /* Allocated in stack frame of square() */

  result = x * x;
  return result;    /* Return value passed via register */
}

static void doCalc(int val)    /* Allocated in stack frame of doCalc() */
{
  printf("The square of %d is %d\n", val, square(val));

  if(val < 1000){
    int t;    /* Allocated in stack frame of doCalc()*/

    t = val * val * val;
    printf("The cube of %d is %d\n", val, t);
  }

}

int main(int argc, char *argv[])    /* Allocated in stack frame of main() */
{
  static int key = 9973;    /* Initialized Data Segment */
  static char mbuf[10240000];    /* Uninitialized Data Segment */
  char *p;     /* Allocated in stack frame of main() */

  p = malloc(1024);          /* Points to a memory in Heap Segment */

  doCalc(key);

  exit(EXIT_SUCCESS);

}
