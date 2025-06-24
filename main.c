#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "term.h"

void *input_handling(void *arg);

int main()
{
    pthread_t input_thread;
    
    float sleep_time = 1;
    int keepRunning = 1;

    assert(tty_raw(STDIN_FILENO) != -1);

    pthread_create(&input_thread, NULL, input_handling, &keepRunning);

    while (keepRunning) {
	printf(CURSOR_HIDE);
	printf(SCREEN_PUSH);
	printf(CURSOR_HIDE);
	printf(SCREEN_CLEAR);
	printf(CURSOR_HOME);

	printf("Hello, World!\n");

	term_sleep(sleep_time);
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
	    assert(tty_reset(STDIN_FILENO) != -1);
    
	    printf(SCREEN_POP);
	    printf(CURSOR_SHOW);
	    exit(0);
	}
    }

    return NULL;
}
