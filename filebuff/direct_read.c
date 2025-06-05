/** This program is a simple example of using O_DIRECT when opening a file for reading.
 * The arguments it takes , in  order , are for the file to be read, no of bytes to be read from the file, offset to seek before reading the file, and alignment of data buffer to passed to read()
 */

#define _GNU_SOURCE
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd;
    ssize_t numRead;
    size_t length, alignment;
    off_t offset;
    void *buf;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s file length [offset [alignment]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    length = atol(argv[2]); // or strtol(argv[2], NULL, 10);
    offset = (argv > 3) ? atol(argv[3]) : 0;
    alignment = (argv > 4) ? atol(argv[4]) : 4096;
}
