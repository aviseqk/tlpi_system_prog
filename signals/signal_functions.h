#ifndef SIGNAL_FUNCTIONS_H
#define SIGNAL_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>

void printSigset(FILE *of, const char *prefix, const sigset_t *sigset);

int printSigMask(FILE *of, const char *msg);

int printPendingSigs(FILE *of, const char *msg);

#endif
