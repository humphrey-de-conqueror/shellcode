#include <stdio.h> 
#include <unistd.h> 

int main(void)
{
	pid_t pid; 

	pid = getpid(); 

	while (1) {
		printf("people sad before penguin born [pid: %d] \n", pid);
		fflush(stdout);
		sleep(2);
	}

	return 0; 
}