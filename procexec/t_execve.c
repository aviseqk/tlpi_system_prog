#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *argVec[10];
    char *envVec[] = { "GREET=salut", "BYE=adieu", NULL};

    if(argc != 2 || strcmp(argv[1], "--help") == 0) {
        printf("%s pathname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    argVec[0] = strchr(argv[1], '/');
    if (argVec[0] != NULL)
        argVec[0]++;
    else
        argVec[0] = argv[1];
    argVec[1] = "hello world";
    argVec[2] = "goodbye";
    argVec[3] = NULL;

    execve(argv[1], argVec, envVec);
    perror("execve");
    exit(EXIT_FAILURE);
}