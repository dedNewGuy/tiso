/*
  This code was copied from Mark Harrison in StackOverflow to change
  terminal mode to raw mode and reset it back to cooked mode.
  https://stackoverflow.com/a/220876
*/

#include "term.h"

static struct termios   save_termios;
static int              term_saved;

int tty_raw(int fd)
{
    struct termios  buf;

    if (tcgetattr(fd, &save_termios) < 0) /* get the original state */
        return -1;

    buf = save_termios;

    buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* echo off, canonical mode off, extended input
       processing off, signal chars off */

    buf.c_iflag &= ~(BRKINT | ICRNL | ISTRIP | IXON);
    /* no SIGINT on BREAK, CR-toNL off, input parity
       check off, don't strip the 8th bit on input,
       ouput flow control off */

    buf.c_cflag &= ~(CSIZE | PARENB);
    /* clear size bits, parity checking off */

    buf.c_cflag |= CS8;
    /* set 8 bits/char */

    buf.c_oflag &= ~(OPOST);
    /* output processing off */

    buf.c_cc[VMIN] = 1;  /* 1 byte at a time */
    buf.c_cc[VTIME] = 0; /* no timer on input */

    if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
        return -1;

    term_saved = 1;

    return 0;
}


int tty_reset(int fd)
{
    if (term_saved)
        if (tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
            return -1;

    return 0;
}
