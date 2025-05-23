#include <stdarg.h>
#include "error_functions.h"
#include "tlpi_hdr.h"
#include "ename.c.inc"	/* defines ename and MAX_ENAME */

#ifdef __GNUC__
__attribute__ ((__noreturn__))
#endif
static void terminate(Boolean useExit3)
{
	char *s;

	/* dump core if EF_DUMPCORE environment variable is defined and is a non empty string otherwise call exit(3) or _exit(2) depending on value of useExit3. */
	s = getenv("EF_DUMPCORE");

	if(s!= NULL && *s != '\0')
		abort();
	else if(useExit3)
		exit(EXIT_FAILURE);
	else
		_exit(EXIT_FAILURE);
}

static void outputError(Boolean useErr, int err, Boolean flushStdout, const char *format, va_list ap)
{
	#define BUF_SIZE 500
	char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];

	vsnprintf(userMsg, BUF_SIZE, format, ap);

	if(useErr)
		snprintf(errText, BUF_SIZE, " [%s %s]", (err > 0 && err <= MAX_ENAME) ? ename[err] : "?UNKNOWN?", strerror(err));
	else
		snprintf(errText, BUF_SIZE, ":");

	snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);

	if(flushStdout)
		fflush(stdout);
	fputs(buf, stderr);
	fflush(stderr);
}

/* this method writes to the error with the err no */
void errMsg(const char *format)
{
	va_list argList;
	int savedErrno;

	savedErrno = errno;

	va_start(argList, format);
	outputError(TRUE, errno, TRUE, argList);
	va_end(argList);

	errno = savedErrno;
}
/* this method writes to the error with the err no but exits the program afterwards as well */
void errExit(const char *format)
{
	va_list argList;

	va_start(argList, format);
	outputError(TRUE, errno, TRUE, argList);
	va_end(argList);

	terminate(TRUE);
}

void err_exit(const char *format)
{
	va_list argList;

	va_start(argList, format);
	outputError(TRUE, errno, FALSE, argList);
	va_end(argList);

	terminate(FALSE);
}

void errExitEN(int errnum, const char *format, ...)
{
	va_list argList;

	va_start(argList, format);
	outputError(TRUE, errnum, TRUE, format, argList);
	va_end(argList);

	terminate(TRUE);
}

void fatal(const char *format)
{
	va_list argList;

	va_start(argList, format);
	outputError(FALSE, 0, TRUE, format, argList);
	va_end(argList);

	terminate(TRUE);
}

void usageErr(const char *format, ...)
{
	va_list argList;

	fflush(stdout);	/* Flush any pending stdout */
	fprintf(stderr, "Usage: ");
	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);

	fflush(stderr);
	exit(EXIT_FAILURE);
}

void cmdLineErr(const char *format, ...)
{
	va_list argList;

	fflush(stdout);	/* Flush any pending stdout */
	fprintf(stderr, "Command-line usage error: ");
	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);

	fflush(stderr);	/* In case stderr is not line-buffered */
	exit(EXIT_FAILURE);
}
