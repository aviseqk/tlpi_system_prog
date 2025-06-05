/* The purpose of this program is to showcase a small difference between write() system call and
  printf() library function and the way in which kernel outputs the data for these cases
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
  // since the printf is neither being ended by a newline or \0, it will get flushed after the write call
  // even though it was called before it, because kernel flushes the data from its buffer at its own pace,
  // but write forces it to flush the data immediately.
  printf("To man the world is twofold, ");
  write(STDOUT_FILENO, "in accordance with his twofold attitude.\n", 41);

  // now case 2 where we force the kernel to flush the data when printf is called
  // here we can see the printf statement gets flushed to stdout and then the write one happens
  printf("\nTo man the world is twofold, ");
  fflush(stdout);
  write(STDOUT_FILENO, "in accordance with his twofold attitude.\n", 41);
 
  exit(EXIT_SUCCESS);      
}
