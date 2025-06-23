#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static volatile int keepRunning = 1;

void INTHandler(int sig);

int main()
{

    signal(SIGINT, INTHandler);
    
    float fps = 1 / 60;
    while (keepRunning) {
	printf("\033[H");
	printf("\033[?25l");
	printf("\033[2J");
	printf("Hello, World!\n");
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
