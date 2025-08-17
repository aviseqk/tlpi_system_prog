#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>
#include "read_line.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define PORT_NUM "50000"	// Port number of the server

#define INT_LEN 30	// size of string able to hold largest integer
