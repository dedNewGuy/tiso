#include "term.h"

static struct termios   save_termios;
static int              term_saved;

int tty_noncanon(void)
{
    struct termios  buf;

    if (tcgetattr(STDIN_FILENO, &save_termios) < 0)
        return -1;

    buf = save_termios;

    buf.c_lflag &= ~(ECHO | ICANON);

    /*
      Here, read is block until read() get 1 byte of input
     */
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0; 

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &buf) < 0)
        return -1;

    term_saved = 1;

    return 0;
}


int reset_tty_mode(void)
{
    if (term_saved)
        if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &save_termios) < 0)
            return -1;

    return 0;
}
