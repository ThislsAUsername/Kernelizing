/*
 * fork_long_tail.c
 *
 *  Created on: Dec 25, 2015
 *      Author: duolu
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


#include "sig_handling.c"


int main(int argc, char **argv)
{
	int i;
	pid_t pid;

	if(argc < 2) {
		printf("Usage: fork_long_tail n\n");
		return 0;
	}

        register_handlers();

	int n = strtol(argv[1], NULL, 10);

	int p = getpid();
	printf("%d\n", p);
	fflush(stdout);

	char new_n[4];
	snprintf(new_n, sizeof(new_n), "%d", n - 1);	

	if(n > 0) {

    		pid = fork();
    		if(pid == 0){
    			execl(argv[0], argv[0], new_n, NULL);
    		}
	}
 
    	while(1)
    		;
	return 0;
}

