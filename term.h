#ifndef _TERM_H
#define _TERM_H

#include <unistd.h>
#include <termios.h>

#define SCREEN_CLEAR "\033[2J"
#define SCREEN_PUSH "\033[?1049h"
#define SCREEN_POP "\033[?1049l"

#define CURSOR_HOME "\033[H"
#define CURSOR_HIDE "\033[?25l"
#define CURSOR_SHOW "\033[?25h"

#define term_sleep(seconds) (sleep(seconds))

int tty_noncanon();
int reset_tty_mode();
    
#endif //_TERM_H
