#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "term.h"

void *input_handling(void *arg);
void restore_term_state(void);

void timer_descend(int *hour, int *minute, int *second);

int main(void)
{
    pthread_t input_thread;
    
    float sleep_time = 1;
    int keepRunning = 1;

    assert(tty_noncanon() != -1);

    pthread_create(&input_thread, NULL, input_handling, &keepRunning);

    int hour = 1;
    int minute = 0;
    int second = 0;

    char frame_buffer[64];

    while (keepRunning) {
	printf(CURSOR_HIDE);
	printf(SCREEN_PUSH);
	printf(CURSOR_HIDE);
	printf(SCREEN_CLEAR);
	printf(CURSOR_HOME);

	sprintf(frame_buffer, "%02d:%02d:%02d", hour, minute, second);
	printf("%s\n", frame_buffer);

	term_sleep(sleep_time);

	timer_descend(&hour, &minute, &second);
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
