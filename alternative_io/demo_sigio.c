/* This program is a small example explaining how signal driven I/O works
It does by employing the signal driven IO mechanism as required and then it places the terminal in a cbreak mode 
The program then enters an infinite loop, performing the work of incrementing a variable cnt, while waiting for input to become available
Wheneber the input becomes available, the signal handler sets a flag `gotSigio`, that is being monitored by the main loop
When the main program sees this signal, it reads all the available input characters, and prints them alongside the current value of the ever-so-incrementing int ( an imitation of main loop doing other tasks )
If a hash character is inputted, the program terminates then and there.
*/

#include <fcntl.h>
#include <termios.h>
#include "../terminals/tty_functions.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>

static volatile sig_atomic_t gotSigio = 0;  /* set non zero on receipt of SIGIO signal */

static void sigioHandler(int sig)
{
    gotSigio = 1;  /* set flag to indicate signal received */
}

int main(int argc, char *argv[])
{
    int flags, j, cnt;
    struct termios origTermios;
    char ch;
    struct sigaction sa;
    bool done;

    /* Establish handler for "I/O possible signal" */
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sigioHandler;
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    /* set owner process that is to receive the "I/O possible signal" */
    if (fcntl(STDIN_FILENO, F_SETOWN, getpid()) == -1) {
        perror("fcntl(F_SETOWN)");
        exit(EXIT_FAILURE);
    }

    /* enable "I/O Possible" signaling and make I/O non blocking for fd */
    flags = fcntl(STDIN_FILENO, F_GETFL);
    if (fcntl(STDIN_FILENO, F_SETFL, flags | O_ASYNC | O_NONBLOCK ) == -1) {
        perror("fcntl(SET_FL)");
        exit(EXIT_FAILURE);
    }

    /* Place terminal in cbreak mode */
    if (ttySetCbreak(STDIN_FILENO, &origTermios) == -1) {
        perror("ttySetCBreak");
        exit(EXIT_FAILURE);
    }

    for (done = false, cnt = 0; !done ; cnt++) {
        for (j = 0; j < 100000000; j++)
            continue;                   /* Slow main loop down a little */

        if (gotSigio) {                 /* Is input available? */
            gotSigio = 0;

            /* Read all available input until error (probably EAGAIN)
               or EOF (not actually possible in cbreak mode) or a
               hash (#) character is read */

            while (read(STDIN_FILENO, &ch, 1) > 0 && !done) {
                printf("cnt=%d; read %c\n", cnt, ch);
                done = ch == '#';
            }
        }
    }

    /* Restore original terminal settings */

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) == -1) {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);

}