/* Talking to A Shell Command via Pipes - how to use popen() and pclose() - Refer TLPI44.5 for details */

#include "../procexec/print_wait_status.h"
#include <ctype.h>
#include <limits.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define POPEN_FMT "/bin/ls -d %s 2> /dev/null"
#define PAT_SIZE 50
#define PCMD_BUF_SIZE (sizeof(POPEN_FMT) + PAT_SIZE)

int main(int argc, char *argv[])
{
	char pat[PAT_SIZE];
	char popenCmd[PCMD_BUF_SIZE];
	FILE *fp;
	bool badPattern;
	int len, status, fileCnt, j;
	char pathname[PATH_MAX];

	for (;;){	// read pattern, display results of globbing
		printf("pattern: ");
		fflush(stdout);
		if (fgets(pat, PAT_SIZE, stdin) == NULL)
			break;	// EOF
		len = strlen(pat);
		if (len <= 1)	// empty line
			continue;

		if (pat[len-1] == '\n')		/* strip the traling new line */
			pat[len-1] = '\0';

		/* ensure that the pattern contains only valid characters, like letters, digits, underscore, dot and shell globbing characters */
		for (j = 0, badPattern = false; j < len && !badPattern; j++){
			if (!isalnum((unsigned char) pat[j]) && strchr("_*?[^-].", pat[j]) == NULL)
				badPattern = true;
		}

		if (badPattern) {
			printf("Bad pattern character: %c\n", pat[j - 1]);
			continue;
		}

		/* build and execute command to glob pat */
		
		snprintf(popenCmd, PCMD_BUF_SIZE, POPEN_FMT, pat);

		fp = popen(popenCmd, "r");
		if (fp == NULL) {
			printf("popen() failed\n");
			continue;
		}

		/* read resulting list of pathnames until EOF */
		fileCnt = 0;
		while(fgets(pathname, PATH_MAX, fp) != NULL) {
			printf("%s", pathname);
			fileCnt++;
		}

		/* close the pipe and display the termination status */
		status = pclose(fp);
		printf("	%d matching file%s\n", fileCnt, (fileCnt != 1) ? "s" : "");
		printf("	pclose() status = %#x\n", (unsigned int) status);
		
		if (status != 1)
			printWaitStatus("\t", status);
	}

	exit(EXIT_SUCCESS);
}
