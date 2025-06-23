#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

static volatile int keepRunning = 1;

void INTHandler(int sig);

int main(void)
{

    signal(SIGINT, INTHandler);

    time_t t;
    struct tm b_time;
    
    float fps = 1;

    while (keepRunning) {
	/* Handle screen clearing */
	printf("\033[10;10H");
	printf("\033[?25l");
	printf("\033[2J");

	t = time(NULL);
	b_time = *localtime(&t);

	printf("now: %d-%02d-%02d %02d:%02d:%02d\n", b_time.tm_year + 1900
	       , b_time.tm_mon + 1, b_time.tm_mday, b_time.tm_hour, b_time.tm_min, b_time.tm_sec);
	
	/* Sleep for 1 seconds */
	sleep(fps);
    }
    
    return 0;
}


void INTHandler(int sig)
{
    signal(sig, SIG_IGN);
    keepRunning = 0;
    printf("\033[?25h");   
}
