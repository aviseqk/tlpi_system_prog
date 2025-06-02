/* This program showcases the sysconf() library function to retrieve and display the system set limits constants 
 * defined in the limits.h header file */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>

// Prints msg alongwith the sysconf value for 'name' limit
static void sysconfPrint(const char *msg, int name)
{
	long lim;
	errno = 0;
	lim = sysconf(name);
 	// call succeeded, limit is determinate
 	if (lim != -1) {
 		printf("%s %ld\n", msg, lim);
	} else {
 		if (errno == 0) // Call succeeded, limit indeterminate
 			printf("%s (indeterminate)\n", msg);
 		else {
		// Call failed
 			perror("sysconf");
			exit(-1);
 		}
	}
}

int main(int argc, char *argv[])
{
	sysconfPrint("_SC_ARG_MAX:          ", _SC_ARG_MAX);
	sysconfPrint("_SC_LOGIN_NAME_MAX:   ", _SC_LOGIN_NAME_MAX);
	sysconfPrint("_SC_OPEN_MAX:         ", _SC_OPEN_MAX);
	sysconfPrint("_SC_NGROUPS_MAX:      ", _SC_NGROUPS_MAX);
	sysconfPrint("_SC_PAGESIZE:         ", _SC_PAGESIZE);
	sysconfPrint("_SC_RTSIG_MAX         ", _SC_RTSIG_MAX);
	sysconfPrint("_SC_SEMAPHORES:       ", _SC_SEMAPHORES);
	sysconfPrint("_SC_THREADS:          ", _SC_THREADS);

	exit(EXIT_SUCCESS);
}
