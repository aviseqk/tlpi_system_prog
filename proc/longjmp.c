/*
 * The purpose of this program is to demonstrate the way setjmp() and longjmp() library functions work
 * setjmp() -> used to set and mark a place where we plan to return the execution flow later in time.
 * it saves the current stack pointer register and program instruction pointer in env variables for hardsaving
 * longjmp() -> the function call that leads to return to an earlier set non-local location
 *
 * NOTE 1: This is done for a non-local location in our program i.e. a function other than the currently executing function
 * if the place to jump to is a local, i.e. within the same scope we can jump using 'goto' as the stack frame for this function
 * will currently be on the stack data segment for sure
 * But if, the function is a different function altogether there's no way for C to know if the stack frame for the target function
 * is residing on the stack or not. As C does not allow nested functions or their declarations, so not sure if stack would be correct
 *
 * NOTE 2: 
 *        Case 1: The first or normal return from setjmp() call would return 0, as the value
 *        Case 2: The second or post-jump return would return a faked integer that would be provided in longjmp() call
 *    thus, this distinction helps us distinguish the reasons for return and further actions can be taken based on that
 * Major use case in scenario-specific error handling
 * */

#include <setjmp.h>
#include "../lib/tlpi_hdr.h"

static jmp_buf env;

static void function2(void)
{
  longjmp(env, 2);
}

static void function1(int argc)
{
  if (argc == 1)
    longjmp(env, 1);
  function2();
}

int main(int argc, char *argv[])
{
  switch(setjmp(env)) {
    case 0:
      // This is the return after the initial return
      printf("Calling function1 after initial setjmp()\n");
      function1(argc);
      break;

    case 1:
      // This when we jumped back from function1
      printf("We jumped back from function1()\n");
      break;

    case 2:
      // This when we jumped back from function2
      printf("We jumped back from function2()\n");
      break;
  }
  exit(EXIT_SUCCESS);
}
