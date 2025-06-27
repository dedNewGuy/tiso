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

typedef struct {
    int x;
    int y;
    int width;
    int height;
} viewport_t;

viewport_t init_viewport();


typedef struct {
    int width;
    int height;
} cell_t;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int segment_x;
    int segment_y;
    cell_t cell;
} canvas_t;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int is_visible;
} rect_t;

rect_t init_rect(int x, int y, int width, int height, int is_visible);

typedef struct {
    int top;
    int top_left;
    int top_right;
    int middle;
    int bottom;
    int bottom_left;
    int bottom_right;
} digit_rect_t;

canvas_t init_canvas(viewport_t viewport, int segment_x, int segment_y);
void canvas_resize(viewport_t *viewport, canvas_t *canvas);
void canvas_render_box(canvas_t canvas, int x, int y);
void canvas_render_rect(canvas_t canvas, rect_t rect);
void canvas_render_digit(canvas_t canvas, digit_rect_t digit);
void canvas_render_cell(canvas_t canvas, int index_x, int index_y);

#endif //_TERM_H
