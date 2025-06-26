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

void *input_handling(void *arg);
void restore_term_state(void);

void timer_descend(int *hour, int *minute, int *second);
void render_timer(char frame_buffer[], int hour, int minute, int second);
int is_time_correct_format(char *val);

#define CANVAS_OFFSET_X 10
#define CANVAS_OFFSET_Y 5



int main(int argc, char **argv)
{
    viewport_t viewport = init_viewport();
    canvas_t canvas = init_canvas(viewport.x + CANVAS_OFFSET_X,
				  viewport.y + CANVAS_OFFSET_Y,
				  viewport.width - CANVAS_OFFSET_X,
				  viewport.height - CANVAS_OFFSET_Y);
    
    int hour = 0;
    int minute = 0;
    int second = 0;

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
	char *second_str = strtok(NULL, ":");
	
	if (is_time_correct_format(hour_str) && is_time_correct_format(minute_str)
	    && is_time_correct_format(second_str)) {
	    hour = atoi(hour_str);
	    minute = atoi(minute_str);
	    second = atoi(second_str);
	} else {
	    log_errorf("Only digit is allowed");
	    exit(1);
	}
    } else {
	log_errorf("No Option supplied");
	exit(1);
    }
    
    pthread_t input_thread;
    
    float sleep_time = 1;
    int keepRunning = 1;

    assert(tty_noncanon() != -1);

    pthread_create(&input_thread, NULL, input_handling, &keepRunning);


    char frame_buffer[64];

    while (keepRunning) {
	printf(CURSOR_HIDE);
	printf(SCREEN_PUSH);
	printf(CURSOR_HIDE);
	printf(SCREEN_CLEAR);
	printf(CURSOR_HOME);

	/* render_timer(frame_buffer, hour, minute, second); */

	for (int y = 0; y < 3; ++y) {
	    canvas_render_box(canvas, 0, y);
	    canvas_render_box(canvas, 1, y);
	    canvas_render_box(canvas, 2, y);
	    canvas_render_box(canvas, 3, y);
	}

	term_sleep(sleep_time); // Sleep for sleep_time second

	/* timer_descend(&hour, &minute, &second); */
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

int is_time_correct_format(char *val)
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
