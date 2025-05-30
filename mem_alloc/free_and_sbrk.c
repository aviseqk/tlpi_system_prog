/*
  * The purpose of this program is to illustrate the effect of free()
  * on the program break. we get the current program break using sbrk(0)
  * call.
  * This program allocates multiple blocks of memory and then releases them
  * in separate patterns to show the impact on the program break based on cmdLine args
  * arg1 ->  number of blocks | arg2 -> size of blocks
  * arg3 -> loop step unit | arg4 -> min unit of the delete range
  * arg5 -> max unit of the delete range 
*/
//#include "../lib/tlpi_hdr.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#define MAX_ALLOCS 1000000
#define GN_GT_O 02

int main(int argc, char *argv[])
{
  char *ptr[MAX_ALLOCS];
  int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

  printf("\n");
  if (argc < 3 || strcmp(argv[1], "--help") == 0){
    printf("%s num-allocs block-size [step [min [max]]]\n", argv[0]);
    exit(-1);
    }

  printf("Initial Program Break:    %10p\n", sbrk(0));
  numAllocs = atoi(argv[1]);

  if (numAllocs > MAX_ALLOCS){
    printf("num-allocs > %d\n", MAX_ALLOCS);
    exit(-1);
  }

  blockSize = atoi(argv[2]);

  freeStep = ( argc > 3 ) ? atoi(argv[3]) : 1;  
  freeMin = ( argc > 4 ) ? atoi(argv[4]) : 1;
  freeMax = ( argc > 5 ) ? atoi(argv[5]) : numAllocs;

  if (freeMax > numAllocs){
    printf("free-max > num-allocs\n");
    exit(-1);
    }

  printf("Initial Program Break:    %10p\n", sbrk(0));

  printf("Allocating %d*%d bytes\n", numAllocs, blockSize);

  for (j = 0; j < numAllocs; j++){
    ptr[j] = malloc(blockSize);
    if(ptr[j] == NULL){
      printf("malloc");
      exit(-1);
    }
  }

  printf("Program Break is now:    %10p\n", sbrk(0));

  printf("Freeing blocks from %d to %d in steps of %d\n", freeMin, freeMax, freeStep);

  for(j = freeMin - 1; j < freeMax; j += freeStep){
    free(ptr[j]);
  }
  printf("Program Break after free() is    %10p\n", sbrk(0));

  exit(EXIT_SUCCESS);
}
