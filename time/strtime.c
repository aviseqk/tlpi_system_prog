/* This program demonstrates the use of strptime() and strftime() functions
 * which are used to convert between broken-down time and a printable format string
 * for the time values.
 * This program takes two/three arguments
 * Argument 1: The time in printable string format
 * Argument 2: The format in which the Argument 1 is provided
 * Argument 3: (optional) The string format in which we want time to be converted
 * */

#define _X_OPEN_SOURCE
#include <time.h>
#include <locale.h>
#include "../lib/tlpi_hdr.h"

#define SBUF_SIZE 1000

int main(int argc, char *argv[])
{
  struct tm tm;
  char sbuf[SBUF_SIZE];
  char *ofmt;
  
  if (argc < 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s input-date-time in-format [out-format]\n", argv[0]);
  
  if (setlocale(LC_ALL, "") == NULL)
    errExit("setlocale");

  memset(&tm, 0, sizeof(struct tm)); // Initialize tm
  if ( (strptime(argv[1], argv[2], &tm) == NULL) )
    fatal("strptime");

  tm.tm_isdst = -1;

  printf("calendar time - in seconds since Epoch: %ld\n", (long) mktime(&tm));

  ofmt = (argc > 3) ? argv[3] : "%H:%M:%S %A, %d %B %Y %Z";
  if (strftime(sbuf, SBUF_SIZE, ofmt, &tm) == 0)
    fatal("strftime returned 0");

  printf("strftime() yields: %s\n", sbuf);

  exit(EXIT_SUCCESS);
}
