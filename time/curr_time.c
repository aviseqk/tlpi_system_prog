/* This program creates a function that returns a string containing the current time 
 * formatted in the specification provided in the format argument
 * This is like a helper function that we use in later programs to get string time
*/
#include <time.h>
#include "curr_time.h"

#define BUF_SIZE 1000
  
char *currTime(const char *format)
{
  static char buf[BUF_SIZE];
  time_t t;
  size_t s;
  struct tm *tm;
  /* Nonreentrant */
  t = time(NULL);
  tm = localtime(&t);
  if (tm == NULL)
    return NULL;
  s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);
  return (s == 0) ? NULL : buf;
}
