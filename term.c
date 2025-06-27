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

static struct winsize win;

viewport_t init_viewport()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
    return (viewport_t){
	.x = 0,
	.y = 0,
	.height = win.ws_row,
	.width = win.ws_col,
    };
}

canvas_t canvas_resize(viewport_t *viewport)
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &win);
    viewport->width = win.ws_col;
    viewport->height = win.ws_row;
    
    canvas_t canvas = init_canvas(*viewport, 27, 7);
    return canvas;
}

canvas_t init_canvas(viewport_t viewport, int segment_x, int segment_y)
{
    // TODO: Proper handling for resizing. Proper x and y I meant.
    // TODO: How to decide a good cell width and height base on canvas size.
    int x = (0.5 * viewport.width) - (segment_x + 1);
    int y = 0.25 * viewport.height;
    int cell_width = 2;
    int cell_height = 1;
    int width = segment_x * cell_width;
    int height = segment_y * cell_height;
    canvas_t canvas = {
	.x = x,
	.y = y,
	.width = width,
	.height = height,
	.segment_x = segment_x,
	.segment_y = segment_y,
	.cell = (cell_t){
	    .height = cell_height,
	    .width = cell_width,
	}

    };

    return canvas;
}

#define PIXEL "\u2588\n"

void canvas_render_box(canvas_t canvas, int x, int y)
{
    printf("\033[%d;%dH", canvas.y + y, canvas.x + x);
    printf(PIXEL);
}

rect_t init_rect(int x, int y, int width, int height, int is_visible)
{
    rect_t rect = {
	.x = x,
	.y = y,
	.width = width,
	.height = height,
	.is_visible = is_visible
    };
    return rect;
}

void canvas_render_rect(canvas_t canvas, rect_t rect)
{
    if (rect.is_visible == 0) return;
    for (int row = 0; row < rect.height; ++row) {
	int y = row + rect.y;
	for (int col = 0; col < rect.width; ++col) {
	    int x = col + rect.x;
	    canvas_render_box(canvas, x, y);
	}
    }
}

void canvas_render_cell(canvas_t canvas, int index_x, int index_y)
{
    int canvas_x_start = index_x * canvas.cell.width;
    int canvas_y_start = index_y * canvas.cell.height;

    rect_t rect = init_rect(canvas_x_start, canvas_y_start, canvas.cell.width,
			    canvas.cell.height, 1);

    canvas_render_rect(canvas, rect);
}

void canvas_render_digit(canvas_t canvas, int offset)
{
    	// TOP
	canvas_render_cell(canvas, 0 + offset, 0);
	canvas_render_cell(canvas, 1 + offset, 0);
	canvas_render_cell(canvas, 2 + offset, 0);

	// TOP LEFT
	canvas_render_cell(canvas, 0 + offset, 1);
	canvas_render_cell(canvas, 0 + offset, 2);

	// TOP RIGHT
	canvas_render_cell(canvas, 2 + offset, 1);
	canvas_render_cell(canvas, 2 + offset, 2);

	// MIDDLE
	canvas_render_cell(canvas, 0 + offset, 3);
	canvas_render_cell(canvas, 1 + offset, 3);
	canvas_render_cell(canvas, 2 + offset, 3);

	// BOTTOM LEFT
	canvas_render_cell(canvas, 0 + offset, 4);
	canvas_render_cell(canvas, 0 + offset, 5);

	// BOTTOM RIGHT
	canvas_render_cell(canvas, 2 + offset, 4);
	canvas_render_cell(canvas, 2 + offset, 5);

	// BOTTOM
	canvas_render_cell(canvas, 0 + offset, 6);
	canvas_render_cell(canvas, 1 + offset, 6);
	canvas_render_cell(canvas, 2 + offset, 6);
}
