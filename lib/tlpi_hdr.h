#ifndef TLPI_HDR_H
#define TLPI_HDR_H  /* prevent accidental double inclusion */

#include <sys/types.h>	/* Type definitions used by many programs */
#include <stdio.h>	/* glib Standard I/O Functions*/
#include <stdlib.h>	/* Prototypes of commonly used library functions */
#include <unistd.h>	/* Prototypes for System Calls */
#include <errno.h>	/* Declares errno and defines error constants */
#include <string.h>	/* String handling Functions */

#include "get_num.h"	/* declares our funcstions for handling numeric arguments */
#include "error_functions.h"	/* declares our error-handling functions */

typedef enum { FALSE, TRUE } Boolean;

#define min(m,n) ((m) < (n) ? (m) : (n))
#define max(m,n) ((m) > (n) ? (m) : (n))

#endif
