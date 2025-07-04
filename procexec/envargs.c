#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

extern char **environ;

int main(int argc, char *argv[])
{
    int j;
    char **ep;

    for (j=0;j<argc;j++)
        printf("argv[%d] = %s\n", j, argv[j]);

    for (ep=environ; *ep != NULL; ep++)
        printf("environ: %s\n", *ep);

    exit(EXIT_SUCCESS);
}