#include <sys/ioctl.h>
#include <stdio.h>

#include "term.h"

static struct termios   save_termios;
static int              term_saved;

int tty_noncanon(void)
{
    struct termios  buf;

    if (tcgetattr(STDIN_FILENO, &save_termios) < 0)
        return -1;

    buf = save_termios;

    buf.c_lflag &= ~(ECHO | ICANON | ISIG);

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

viewport_t init_viewport()
{
    struct winsize win;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
    return (viewport_t){
	.x = 0,
	.y = 0,
	.height = win.ws_row,
	.width = win.ws_col,
    };
}

canvas_t init_canvas(int x, int y, int width, int height)
{
    canvas_t canvas = {
	.x = x,
	.y = y,
	.width = width,
	.height = height
    };

    return canvas;
}

#define PIXEL "\u2588\n"

void canvas_render_box(canvas_t canvas, int x, int y)
{
    printf("\033[%d;%dH", canvas.y + y, canvas.x + x);
    printf(PIXEL);
}

void canvas_render_rect(canvas_t canvas, rect_t rect)
{
    for (int row = 0; row < rect.height; ++row) {
	int y = row + rect.y;
	for (int col = 0; col < rect.width; ++col) {
	    int x = col + rect.x;
	    canvas_render_box(canvas, x, y);
	}
    }
}
