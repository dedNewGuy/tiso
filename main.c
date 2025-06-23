#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static volatile int keepRunning = 1;

void INTHandler(int sig);

int main()
{

    signal(SIGINT, INTHandler);

    char buff[64];
    
    float fps = 1;
    int i = 0;
    while (keepRunning) {

	/* Handle screen clearing */
	printf("\033[H");
	printf("\033[?25l");
	printf("\033[2J");
	buff[i] = '+';
	printf("%s\n", buff);
	++i;

	/* Sleep for 1/60 */
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
