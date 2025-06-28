#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "term.h"

// Feel like it is useless
#define log_errorf(...)				\
    fprintf(stderr, "[ERROR] ");		\
    fprintf(stderr, __VA_ARGS__);		\
    fprintf(stderr, "\n");


// TODO: Modularize code so it's not messy
// But then again, at this point I don't care.
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

void parse_config_file(char *buf[5], char *filename);
void parse_pray_time(char *pray_time[5], int *hour, int *minute);

int main(int argc, char **argv)
{
    viewport_t viewport = init_viewport();
    canvas_t canvas = init_canvas(viewport, 27, 7);
    
    int hour = 0;
    int minute = 0;
    int second = 0;

    timef_val_t time_f = {0};

    // If option -t is supplied
    // TODO: Abstract this so it's easier to expand
    // But then again, I'm only planning to finish this project
    // as is
    if (argc == 3) {
	char *opt = argv[1];
	if (strcmp(opt, "-t") != 0) {
	    log_errorf("Unknown option supplied %s", opt);
	    exit(1);
	}
	char *timer_val = argv[2];
	char *hour_str   = strtok(timer_val, ":");
	char *minute_str = strtok(NULL, ":");
	char *second_str = strtok(NULL, ":");
	
	if (is_time_in_digit(hour_str) && is_time_in_digit(minute_str)
	    && is_time_in_digit(second_str)) {
	    hour = atoi(hour_str);
	    minute = atoi(minute_str);
	    second = atoi(second_str);
	} else {
	    log_errorf("Only digit is allowed");
	    exit(1);
	}
    } else {
	char *pray_time[5];
	char *home_dir = getenv("HOME");
	char config_filepath[126];
	sprintf(config_filepath, "%s/.config/tiso/config.txt", home_dir);
	parse_config_file(pray_time, config_filepath);
	parse_pray_time(pray_time, &hour, &minute);
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


	term_sleep(sleep_time);
	
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

void parse_config_file(char *buf[5], char *filename)
{

    FILE *config_file = fopen(filename, "r");
    if (config_file == NULL) {
	perror("tiso");
	exit(1);
    }
    
    char s[256];
    int i = 0;
    int pray_time_text_size = 6;
    while ((fgets(s, 256, config_file)) != NULL) {
        strtok(s, "=");
	char *prayer_time = strtok(NULL, "=");
	buf[i] = malloc(pray_time_text_size * sizeof(char));
	if (buf[i] == NULL) {
	    perror("tiso");
	    exit(1);
	}
	strcpy(buf[i], prayer_time);
	++i;
    }
}

/*
  Compare hour and minute for prayer time.
  Return 0 if equal
  Return 1 if time1 > time2
  Return -1 if time1 < time2
 */
int compare_hm(int h1, int m1, int h2, int m2)
{
    if (h1 > h2) {
	return 1;
    } else if (h1 < h2) {
	return -1;
    } else {
	if (m1 > m2) {
	    return 1;
	} else if (m1 < m2) {
	    return -1;
	} else {
	    return 0;
	}
    }
}

void parse_pray_time(char *pray_time[5], int *hour, int *minute)
{
    /* Get current time */
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    int curr_hour = timeinfo->tm_hour;
    int curr_minute = timeinfo->tm_min;

    // Check and compare with isha (because it has special case)
    char *isha_prayt = pray_time[4];
    
    int ipray_h = atoi(strtok(isha_prayt, ":"));
    int ipray_m = atoi(strtok(NULL, ":"));
    int cmp_curr_isha = compare_hm(curr_hour, curr_minute, ipray_h, ipray_m);
    
    /*
      If current_time < pray_time[(i + 1) % 5] get the time difference
      between current_time and pray_time[i]
     */
    for (int i = 0; i < 5; ++i) {
	int index = (i + 1) % 5;
	char *next_prayt = pray_time[index];
	int npray_h = atoi(strtok(next_prayt, ":"));
	int npray_m = atoi(strtok(NULL, ":"));

	int is_currt_larger = compare_hm(curr_hour, curr_minute, npray_h, npray_m);

	if (is_currt_larger < 0 || cmp_curr_isha > 0) {
	    char *prayt = pray_time[i];
	    int pray_h = atoi(strtok(prayt, ":"));
	    int pray_m = atoi(strtok(NULL, ":"));
	    
	    int delta_h = pray_h - curr_hour;
	    int delta_m = pray_m - curr_minute;
	    if (delta_m < 0) {
		delta_m += 60;
		delta_h -= 1;
	    }
	    if (delta_h < 0) {
		delta_h += 24;
	    }
	    *hour = delta_h;
	    *minute = delta_m;
	    break;
	}
    }
}
