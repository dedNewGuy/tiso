#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

#include "term.h"

#define log_errorf(...)				\
    fprintf(stderr, "[ERROR] ");		\
    fprintf(stderr, __VA_ARGS__);		\
    fprintf(stderr, "\n");


typedef struct {
    int hour_one;
    int hour_ten;
    
    int minute_one;
    int minute_ten;
    
    int second_one;
    int second_ten;
} timef_val_t;

void *input_handling(void *arg);
void restore_term_state(void);

void timer_descend(int *hour, int *minute, int *second);
void update_time(timef_val_t *time_f, int hour, int minute, int second);
void render_timer(char frame_buffer[], int hour, int minute, int second);
int is_time_in_digit(char *val);

int main(int argc, char **argv)
{
    viewport_t viewport = init_viewport();
    canvas_t canvas = init_canvas(viewport, 27, 7);
    
    int hour = 0;
    int minute = 0;
    int second = 0;

    timef_val_t time_f = {0};
    
    if (argc == 3) {
	char *opt = argv[1];
	if (strcmp(opt, "-t") != 0) {
	    log_errorf("Unknown option supplied %s", opt);
	    exit(1);
	}
	char *timer_val = argv[2];
	// TODO: Handle different format. Currently it hardcoded into strictly HH:MM:SS.
	// Causing Seg fault if given otherwise. Need proper error handling
	char *hour_str   = strtok(timer_val, ":");
	char *minute_str = strtok(NULL, ":");
	char *second_str = strtok(NULL, ":"); // [0,0]
	
	if (is_time_in_digit(hour_str) && is_time_in_digit(minute_str)
	    && is_time_in_digit(second_str)) {
	    hour = atoi(hour_str);
	    minute = atoi(minute_str);
	    second = atoi(second_str);
	} else {
	    log_errorf("Only digit is allowed");
	    exit(1);
	}

	// TODO: Maybe handle if minute and second past 60? but it's a cool feature
	// what if someone just want to set 90 second and don't want to think about
	// Turning it to minute and second... Think about it

    } else {
	log_errorf("No Option supplied");
	exit(1);
    }
    
    pthread_t input_thread;
    
    float sleep_time = 1;
    int keepRunning = 1;

    assert(tty_noncanon() != -1);

    pthread_create(&input_thread, NULL, input_handling, &keepRunning);

    digit_rect_t digit_rect = load_digit_rect();
    
    while (keepRunning) {
	printf(CURSOR_HIDE);
	printf(SCREEN_PUSH);
	printf(CURSOR_HIDE);
	printf(SCREEN_CLEAR);
	printf(CURSOR_HOME);

	update_time(&time_f, hour, minute, second);

	/* --  Trying to render 88:88:88  -- */
	digit_rect_set_num(&digit_rect, time_f.hour_ten);
	canvas_render_digit(canvas, 0, digit_rect);
	digit_rect = load_digit_rect();
	
	digit_rect_set_num(&digit_rect, time_f.hour_one);
	canvas_render_digit(canvas, 4, digit_rect);
	digit_rect = load_digit_rect();

	canvas_render_cell(canvas, 8, canvas.segment_y / 2 - 1);
	canvas_render_cell(canvas, 8, canvas.segment_y / 2 + 1);

	digit_rect_set_num(&digit_rect, time_f.minute_ten);
	canvas_render_digit(canvas, 10, digit_rect);
	digit_rect = load_digit_rect();
	
	digit_rect_set_num(&digit_rect, time_f.minute_one);	
	canvas_render_digit(canvas, 14, digit_rect);
	digit_rect = load_digit_rect();

	canvas_render_cell(canvas, 18, canvas.segment_y / 2 - 1);
	canvas_render_cell(canvas, 18, canvas.segment_y / 2 + 1);

	digit_rect_set_num(&digit_rect, time_f.second_ten);
	canvas_render_digit(canvas, 20, digit_rect);
	digit_rect = load_digit_rect();

	digit_rect_set_num(&digit_rect, time_f.second_one);	
	canvas_render_digit(canvas, 24, digit_rect);
	digit_rect = load_digit_rect();	

	// TODO: Improve loop
	term_sleep(sleep_time); // Sleep for sleep_time second
	
	timer_descend(&hour, &minute, &second);

	canvas = canvas_resize(&viewport);
    }
    
    return 0;
}

void *input_handling(void *keepRunning)
{
    char buf[5];
    size_t nbytes = sizeof(buf);

    int run = *(int*)keepRunning;

    while (run) {
	read(STDIN_FILENO, buf, nbytes);
	if ((int)buf[0] == 3 || buf[0] == 'q') {
	    restore_term_state();
	}
    }

    return NULL;
}

void restore_term_state(void)
{
    assert(reset_tty_mode() != -1);
    
    printf(SCREEN_POP);
    printf(CURSOR_SHOW);
    exit(0);
}

void timer_descend(int *hour, int *minute, int *second)
{
    if (*second > 0) {
	*second -= 1;
    } else {
	if (*minute > 0) {
	    *minute -= 1;
	    *second = 59;
	} else {
	    if (*hour > 0) {
		*hour -= 1;
		*minute = 59;
		*second = 59;
	    } else {
		restore_term_state();
	    }
	}
    }
}

void render_timer(char frame_buffer[], int hour, int minute, int second)
{
    sprintf(frame_buffer, "%02d:%02d:%02d", hour, minute, second);
    printf("%s\n", frame_buffer);
}

int is_time_in_digit(char *val)
{
    int len = strlen(val);
    int valid;
    for (int i = 0; i < len; ++i) {
	if (val[i] >= '0' && val[i] <= '9') {
	    valid = 1;
	} else {
	    valid = 0;
	}
    }
    return valid;
}

void update_time(timef_val_t *time_f, int hour, int minute, int second)
{
    time_f->hour_one = hour % 10; 
    time_f->hour_ten = hour * 0.1f;
    time_f->minute_one = minute % 10; 
    time_f->minute_ten = minute * 0.1f;
    time_f->second_one = second % 10; 
    time_f->second_ten = second * 0.1f;
}
