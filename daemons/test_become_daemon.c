/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Supplementary program for Chapter 37 */

/* test_become_daemon.c

   Test our becomeDaemon() function.
*/
#include "become_daemon.h"
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
    becomeDaemon(0);
    /* Normally a daemon would live forever; we just sleep for a while */

    sleep((argc > 1) ? atoi(argv[1]) : 20);

    exit(EXIT_SUCCESS);
}
