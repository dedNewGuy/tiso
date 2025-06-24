#include <stdio.h>
#include <assert.h>
#include "term.h"

int main()
{
    char buf[5];
    size_t nbytes;
    float sleep_time = 1;
    int keepRunning = 1;

    assert(tty_raw(STDIN_FILENO) != -1);


    nbytes = sizeof(buf);
    while (keepRunning) {
	printf(CURSOR_HIDE);
	printf(SCREEN_PUSH);
	printf(CURSOR_HIDE);
	printf(SCREEN_CLEAR);
	printf(CURSOR_HOME);

	printf("Hello, World!\n");
	
	read(STDIN_FILENO, buf, nbytes);
	if ((int)buf[0] == 3 || buf[0] == 'q') {
	    keepRunning = 0;
	}

	term_sleep(sleep_time);
    }

    assert(tty_reset(STDIN_FILENO) != -1);
    
    printf(SCREEN_POP);
    printf(CURSOR_SHOW);
    
    return 0;
}
